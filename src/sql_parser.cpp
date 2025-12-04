#include "../include/sql_ast.h"
#include <algorithm>
#include <cctype>

static std::string trim(const std::string& s){
    size_t b = s.find_first_not_of(" \t\r\n");
    size_t e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) return std::string();
    return s.substr(b, e - b + 1);
}

ParseResult parseSelectSql(const std::string& sql) {
    ParseResult pr; pr.ok = false;
    std::string s = sql;
    // Normalize spaces
    // Simple case-insensitive scan: expect SELECT ... FROM ... [WHERE ...]
    std::string upper = s; std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
    auto posSel = upper.find("SELECT ");
    if (posSel != 0) { pr.error = "Only supports SELECT starting at beginning"; return pr; }
    auto posFrom = upper.find(" FROM ");
    if (posFrom == std::string::npos) { pr.error = "Missing FROM clause"; return pr; }
    std::string cols = trim(s.substr(7, posFrom - 7));
    // Split columns by comma
    if (cols == "*") pr.select.columns = {"*"};
    else {
        size_t start = 0; while (start < cols.size()) {
            auto comma = cols.find(',', start);
            std::string col = trim(cols.substr(start, comma == std::string::npos ? std::string::npos : comma - start));
            if (!col.empty()) pr.select.columns.push_back(col);
            if (comma == std::string::npos) break; else start = comma + 1;
        }
    }
    size_t tableStart = posFrom + 6; // skip " FROM "
    // find WHERE (case-insensitive)
    auto posWhere = upper.find(" WHERE ", tableStart);
    std::string table = trim(s.substr(tableStart, posWhere == std::string::npos ? std::string::npos : posWhere - tableStart));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.select.table = table;
    if (posWhere != std::string::npos) {
        std::string cond = trim(s.substr(posWhere + 7));
        // Support col = literal (literal can be quoted or unquoted token)
        // find '='
        auto eq = cond.find('=');
        if (eq == std::string::npos) { pr.error = "Only supports equality in WHERE"; return pr; }
        std::string left = trim(cond.substr(0, eq));
        std::string right = trim(cond.substr(eq + 1));
        // strip quotes if any
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
    // find table name and column list in parentheses
    size_t posAfter = std::string("CREATE TABLE ").size();
    // locate '('
    size_t lpar = s.find('(', posAfter);
    if (lpar == std::string::npos) { pr.error = "Missing column list"; return pr; }
    std::string table = trim(s.substr(posAfter, lpar - posAfter));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.create.table = table;
    size_t rpar = s.find(')', lpar+1);
    if (rpar == std::string::npos) { pr.error = "Unclosed column list"; return pr; }
    std::string cols = s.substr(lpar+1, rpar - (lpar+1));
    // split by comma and parse "name type [length]"
    size_t start=0;
    while (start < cols.size()){
        // find next comma
        size_t comma = cols.find(',', start);
        std::string item = trim(cols.substr(start, comma==std::string::npos?std::string::npos:comma-start));
        if (!item.empty()){
            // tokenize by spaces
            std::vector<std::string> toks; std::string tok; size_t i=0;
            while (i < item.size()){
                // skip spaces
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
    // Expect: INSERT INTO table VALUES (...)
    if (upper.find("INSERT INTO ") != 0) { pr.error = "Only supports INSERT INTO starting at beginning"; return pr; }
    size_t posAfter = std::string("INSERT INTO ").size();
    // find VALUES
    size_t posValuesUpper = upper.find(" VALUES ", posAfter);
    if (posValuesUpper == std::string::npos) { pr.error = "Missing VALUES clause"; return pr; }
    std::string table = trim(s.substr(posAfter, posValuesUpper - posAfter));
    if (table.empty()) { pr.error = "Missing table name"; return pr; }
    pr.insert.table = table;
    size_t lpar = s.find('(', posValuesUpper);
    size_t rpar = s.find(')', lpar==std::string::npos?posValuesUpper:lpar+1);
    if (lpar == std::string::npos || rpar == std::string::npos || rpar < lpar) { pr.error = "VALUES must have (...)"; return pr; }
    std::string vals = s.substr(lpar+1, rpar - (lpar+1));
    // split by comma but handle quotes simply (no embedded commas in quoted strings assumed)
    size_t start=0; while (start < vals.size()){
        size_t comma = vals.find(',', start);
        std::string item = trim(vals.substr(start, comma==std::string::npos?std::string::npos:comma-start));
        if (!item.empty()){
            // strip quotes
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
