#include "../include/sql_ast.h"
#include <algorithm>
#include <cctype>

#if __has_include("SQLiteLexer.h") && __has_include("SQLiteParser.h")
// ANTLR-based implementation using generated SQLite grammar
#include <antlr4-runtime.h>
#include "SQLiteLexer.h"
#include "SQLiteParser.h"
using namespace antlr4;

static std::string stripQuotes(const std::string &s) {
    if (s.size() >= 2) {
        char c0 = s.front(), c1 = s.back();
        if ((c0 == '\'' && c1 == '\'') || (c0 == '"' && c1 == '"')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}

ParseResult parseSelectSql(const std::string& sql) {
    ANTLRInputStream input(sql);
    SQLiteLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    SQLiteParser parser(&tokens);
    // Entry rule for SELECT in SQLite grammar is 'select_stmt' or 'sql_stmt'
    // We try select_stmt to focus on SELECT
    auto *tree = parser.select_stmt();
    if (!tree || parser.getNumberOfSyntaxErrors() > 0) {
        ParseResult r; r.ok=false; r.error = "SELECT 语法错误"; return r;
    }
    ParseResult res; res.ok=true;
    SqlSelect sel;
    // Minimal extraction: SELECT result columns and FROM table and optional WHERE expr
    // Note: SQLite grammar is complex; here we handle a simplified subset
    // columns: '*' or ID list
    // For simplicity, try to read '*' token in tokens, else collect identifiers from the select core
    bool star=false;
    for (auto t : tokens.getTokens()) {
        if (t->getType() == SQLiteLexer::STAR) { star=true; break; }
    }
    if (star) sel.columns.push_back("*");
    else {
        // Naive: collect IDs as column names before FROM
        for (auto t : tokens.getTokens()) {
            if (t->getType() == SQLiteLexer::IDENTIFIER) {
                sel.columns.push_back(t->getText());
            }
            if (t->getType() == SQLiteLexer::FROM_) break;
        }
        if (sel.columns.empty()) sel.columns.push_back("*");
    }
    // table after FROM
    bool afterFrom=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::FROM_) { afterFrom=true; continue; }
        if (afterFrom && t->getType()==SQLiteLexer::IDENTIFIER){ sel.table = t->getText(); break; }
    }
    // WHERE col = literal
    bool inWhere=false; std::string col; std::string lit; for (size_t i=0;i<tokens.getTokens().size();++i){ auto t=tokens.getTokens()[i];
        if (t->getType()==SQLiteLexer::WHERE_) { inWhere=true; continue; }
        if (inWhere && t->getType()==SQLiteLexer::IDENTIFIER) { col=t->getText(); }
        if (inWhere && t->getType()==SQLiteLexer::ASSIGN) {
            // next literal token
            for (size_t j=i+1; j<tokens.getTokens().size(); ++j){ auto tt=tokens.getTokens()[j];
                if (tt->getType()==SQLiteLexer::NUMERIC_LITERAL || tt->getType()==SQLiteLexer::STRING_LITERAL) { lit = stripQuotes(tt->getText()); break; }
            }
            break;
        }
    }
    if (!col.empty() && !lit.empty()) { sel.where = SqlExpr{col, "=", lit}; }
    if (sel.table.empty()) { res.ok=false; res.error = "缺少表名"; return res; }
    res.select = std::move(sel); return res;
}

ParseCreateResult parseCreateTableSql(const std::string& sql) {
    ANTLRInputStream input(sql);
    SQLiteLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    SQLiteParser parser(&tokens);
    auto *tree = parser.create_table_stmt();
    ParseCreateResult res; res.ok=false;
    if (!tree || parser.getNumberOfSyntaxErrors() > 0) { res.error = "CREATE TABLE 语法错误"; return res; }
    // Using tokens to extract table name and column defs (simplified)
    SqlCreateTable ct;
    // table name after 'CREATE' 'TABLE'
    bool afterTable=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::TABLE_) { afterTable=true; continue; }
        if (afterTable && t->getType()==SQLiteLexer::IDENTIFIER){ ct.table=t->getText(); break; }
    }
    // column list inside parentheses: name type
    bool inParens=false; SqlColumnDef def; bool expectType=false;
    for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::OPEN_PAR) { inParens=true; continue; }
        if (t->getType()==SQLiteLexer::CLOSE_PAR) { inParens=false; continue; }
        if (!inParens) continue;
        if (t->getType()==SQLiteLexer::IDENTIFIER && !expectType) { def = SqlColumnDef{}; def.name = t->getText(); expectType=true; continue; }
        if (expectType && (t->getType()==SQLiteLexer::INT_||t->getType()==SQLiteLexer::FLOAT_||t->getType()==SQLiteLexer::STRING_)){
            def.type = (t->getType()==SQLiteLexer::INT_?"int": t->getType()==SQLiteLexer::FLOAT_?"float":"string");
            ct.columns.push_back(def); expectType=false; continue;
        }
    }
    if (ct.table.empty() || ct.columns.empty()) { res.error = "缺少表名或列定义"; return res; }
    res.ok=true; res.create = std::move(ct); return res;
}

ParseInsertResult parseInsertSql(const std::string& sql) {
    ANTLRInputStream input(sql);
    SQLiteLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    SQLiteParser parser(&tokens);
    auto *tree = parser.insert_stmt();
    ParseInsertResult res; res.ok=false;
    if (!tree || parser.getNumberOfSyntaxErrors() > 0) { res.error = "INSERT 语法错误"; return res; }
    SqlInsert ins;
    // table after INTO
    bool afterInto=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::INTO_) { afterInto=true; continue; }
        if (afterInto && t->getType()==SQLiteLexer::IDENTIFIER){ ins.table=t->getText(); break; }
    }
    // values inside parentheses
    bool inValues=false; for (size_t i=0;i<tokens.getTokens().size();++i){ auto t=tokens.getTokens()[i];
        if (t->getType()==SQLiteLexer::VALUES_) { inValues=true; continue; }
        if (inValues && t->getType()==SQLiteLexer::OPEN_PAR) { continue; }
        if (inValues && t->getType()==SQLiteLexer::CLOSE_PAR) { break; }
        if (inValues && (t->getType()==SQLiteLexer::NUMERIC_LITERAL || t->getType()==SQLiteLexer::STRING_LITERAL)){
            ins.values.push_back(stripQuotes(t->getText()));
        }
    }
    if (ins.table.empty() || ins.values.empty()) { res.error = "缺少表名或值"; return res; }
    res.ok=true; res.insert = std::move(ins); return res;
}

#else
// Fallback: existing handwritten simple parser
static std::string trim(const std::string& s){
    size_t b = s.find_first_not_of(" \t\r\n");
    size_t e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) return std::string();
    return s.substr(b, e - b + 1);
}

ParseResult parseSelectSql(const std::string& sql) {
    ParseResult pr; pr.ok = false;
    std::string s = sql;
    std::string upper = s; std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
    auto posSel = upper.find("SELECT ");
    if (posSel != 0) { pr.error = "Only supports SELECT starting at beginning"; return pr; }
    auto posFrom = upper.find(" FROM ");
    if (posFrom == std::string::npos) { pr.error = "Missing FROM clause"; return pr; }
    std::string cols = trim(s.substr(7, posFrom - 7));
    if (cols == "*") pr.select.columns = {"*"};
    else {
        size_t start = 0; while (start < cols.size()) {
            auto comma = cols.find(',', start);
            std::string col = trim(cols.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
            if (!col.empty()) pr.select.columns.push_back(col);
            if (comma == std::string::npos) break; else start = comma + 1;
        }
    }
    size_t tableStart = posFrom + 6;
    auto posWhere = upper.find(" WHERE ", tableStart);
    std::string table = trim(s.substr(tableStart, posWhere == std::string::npos ? std::string::npos : posWhere - tableStart));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.select.table = table;
    if (posWhere != std::string::npos) {
        std::string cond = trim(s.substr(posWhere + 7));
        auto eq = cond.find('=');
        if (eq == std::string::npos) { pr.error = "Only supports equality in WHERE"; return pr; }
        std::string left = trim(cond.substr(0, eq));
        std::string right = trim(cond.substr(eq + 1));
        if (!right.empty() && (right.front()=='\'' || right.front()=='\"')) {
            char q = right.front();
            if (right.size() >= 2 && right.back()==q) right = right.substr(1, right.size()-2);
        }
        SqlExpr expr{left, "=", right};
        pr.select.where = expr;
    }
    pr.ok = true; return pr;
}

ParseCreateResult parseCreateTableSql(const std::string& sql) {
    ParseCreateResult pr; pr.ok=false;
    std::string s = sql; std::string upper = s;
    std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
    if (upper.find("CREATE TABLE ") != 0) { pr.error = "Only supports CREATE TABLE starting at beginning"; return pr; }
    size_t posAfter = std::string("CREATE TABLE ").size();
    size_t lpar = s.find('(', posAfter);
    if (lpar == std::string::npos) { pr.error = "Missing column list"; return pr; }
    std::string table = trim(s.substr(posAfter, lpar - posAfter));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.create.table = table;
    size_t rpar = s.find(')', lpar+1);
    if (rpar == std::string::npos) { pr.error = "Unclosed column list"; return pr; }
    std::string cols = s.substr(lpar+1, rpar - (lpar+1));
    size_t start=0;
    while (start < cols.size()){
        size_t comma = cols.find(',', start);
        std::string item = trim(cols.substr(start, comma==std::string::npos?std::string::npos:comma-start));
        if (!item.empty()){
            std::vector<std::string> toks; size_t i=0;
            while (i < item.size()){
                while (i<item.size() && std::isspace((unsigned char)item[i])) i++;
                if (i>=item.size()) break;
                size_t j=i; while (j<item.size() && !std::isspace((unsigned char)item[j])) j++;
                toks.push_back(item.substr(i, j-i)); i=j;
            }
            if (toks.size() < 2) { pr.error = "Invalid column def: " + item; return pr; }
            SqlColumnDef cd; cd.name = toks[0]; cd.type = toks[1];
            if (toks.size() >= 3) {
                try { cd.length = std::stoi(toks[2]); } catch(...) { pr.error = "Invalid length in column def: " + item; return pr; }
            } else { cd.length = (cd.type=="string"?255: (cd.type=="int"||cd.type=="float"?4:0)); }
            pr.create.columns.push_back(cd);
        }
        if (comma == std::string::npos) break; else start = comma + 1;
    }
    if (pr.create.columns.empty()) { pr.error = "No columns"; return pr; }
    pr.ok=true; return pr;
}

ParseInsertResult parseInsertSql(const std::string& sql) {
    ParseInsertResult pr; pr.ok=false;
    std::string s = sql; std::string upper=s; std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
    if (upper.find("INSERT INTO ") != 0) { pr.error = "Only supports INSERT INTO starting at beginning"; return pr; }
    size_t posAfter = std::string("INSERT INTO ").size();
    size_t posValuesUpper = upper.find(" VALUES ", posAfter);
    if (posValuesUpper == std::string::npos) { pr.error = "Missing VALUES clause"; return pr; }
    std::string table = trim(s.substr(posAfter, posValuesUpper - posAfter));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.insert.table = table;
    size_t lpar = s.find('(', posValuesUpper);
    size_t rpar = s.find(')', lpar==std::string::npos?posValuesUpper:lpar+1);
    if (lpar == std::string::npos || rpar == std::string::npos || rpar < lpar) { pr.error = "VALUES must have (...)"; return pr; }
    std::string vals = s.substr(lpar+1, rpar - (lpar+1));
    size_t start=0; while (start < vals.size()){
        size_t comma = vals.find(',', start);
        std::string item = trim(vals.substr(start, comma==std::string::npos?std::string::npos:comma-start));
        if (!item.empty()){
            if (!item.empty() && (item.front()=='\'' || item.front()=='\"')){
                char q=item.front(); if (item.back()==q && item.size()>=2) item = item.substr(1, item.size()-2);
            }
            pr.insert.values.push_back(item);
        }
        if (comma == std::string::npos) break; else start = comma + 1;
    }
    if (pr.insert.values.empty()) { pr.error = "No values"; return pr; }
    pr.ok=true; return pr;
}
#endif
