#include "../include/sql_ast.h"
#include <algorithm>
#include <cctype>
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
    auto *tree = parser.select_stmt();
    if (!tree || parser.getNumberOfSyntaxErrors() > 0) {
        ParseResult r; r.ok=false; r.error = "SELECT 语法错误"; return r;
    }
    ParseResult res; res.ok=true;
    SqlSelect sel;
    bool star=false;
    for (auto t : tokens.getTokens()) {
        if (t->getType() == SQLiteLexer::STAR) { star=true; break; }
    }
    if (star) sel.columns.push_back("*");
    else {
        for (auto t : tokens.getTokens()) {
            if (t->getType() == SQLiteLexer::IDENTIFIER) {
                sel.columns.push_back(t->getText());
            }
            if (t->getType() == SQLiteLexer::FROM_) break;
        }
        if (sel.columns.empty()) sel.columns.push_back("*");
    }
    bool afterFrom=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::FROM_) { afterFrom=true; continue; }
        if (afterFrom && t->getType()==SQLiteLexer::IDENTIFIER){ sel.table = t->getText(); break; }
    }
    bool inWhere=false; std::string col; std::string lit; for (size_t i=0;i<tokens.getTokens().size();++i){ auto t=tokens.getTokens()[i];
        if (t->getType()==SQLiteLexer::WHERE_) { inWhere=true; continue; }
        if (inWhere && t->getType()==SQLiteLexer::IDENTIFIER) { col=t->getText(); }
        if (inWhere && t->getType()==SQLiteLexer::ASSIGN) {
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
    SqlCreateTable ct;
    bool afterTable=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::TABLE_) { afterTable=true; continue; }
        if (afterTable && t->getType()==SQLiteLexer::IDENTIFIER){ ct.table=t->getText(); break; }
    }
    bool inParens=false; SqlColumnDef def; bool expectType=false;
    for (size_t i=0; i<tokens.getTokens().size(); ++i){ auto t = tokens.getTokens()[i];
        if (t->getType()==SQLiteLexer::OPEN_PAR) { inParens=true; continue; }
        if (t->getType()==SQLiteLexer::CLOSE_PAR) { inParens=false; continue; }
        if (!inParens) continue;
        if (t->getType()==SQLiteLexer::IDENTIFIER && !expectType) {
            def = SqlColumnDef{}; def.name = t->getText(); expectType=true; continue; }
        if (expectType) {
            std::string tt = t->getText();
            std::string upper = tt; std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
            if (t->getType()==SQLiteLexer::IDENTIFIER) {
                if (upper=="INT" || upper=="INTEGER") def.type = "int";
                else if (upper=="FLOAT" || upper=="REAL" || upper=="DOUBLE") def.type = "float";
                else if (upper=="TEXT" || upper=="STRING") def.type = "string";
                else def.type = tt;
                ct.columns.push_back(def); expectType=false; continue;
            }
            if (t->getType()==SQLiteLexer::COMMA) { continue; }
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
    bool afterInto=false; for (auto t : tokens.getTokens()){
        if (t->getType()==SQLiteLexer::INTO_) { afterInto=true; continue; }
        if (afterInto && t->getType()==SQLiteLexer::IDENTIFIER){ ins.table=t->getText(); break; }
    }
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
