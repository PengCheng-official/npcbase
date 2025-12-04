// Minimal AST structures for SELECT statements
#ifndef NPCBASE_SQL_AST_H
#define NPCBASE_SQL_AST_H

#include <string>
#include <vector>
#include <optional>

struct SqlExpr {
    // Only support column = literal for WHERE
    std::string column;
    std::string op; // '=' only for now
    std::string literal; // store as string; type resolution later
};

struct SqlSelect {
    std::vector<std::string> columns; // '*' or specific columns
    std::string table;
    std::optional<SqlExpr> where;
};

// Minimal CREATE TABLE AST
struct SqlColumnDef {
    std::string name;
    std::string type; // int | float | string
    int length{0};    // for string
};

struct SqlCreateTable {
    std::string table;
    std::vector<SqlColumnDef> columns;
};

// Minimal INSERT AST
struct SqlInsert {
    std::string table;
    std::vector<std::string> values; // literals as strings (quotes stripped)
};

// Parse results
struct ParseResult {
    bool ok{false};
    std::string error;
    SqlSelect select;
};

struct ParseCreateResult {
    bool ok{false};
    std::string error;
    SqlCreateTable create;
};

struct ParseInsertResult {
    bool ok{false};
    std::string error;
    SqlInsert insert;
};

// SELECT
ParseResult parseSelectSql(const std::string& sql);

// CREATE TABLE
ParseCreateResult parseCreateTableSql(const std::string& sql);

// INSERT INTO
ParseInsertResult parseInsertSql(const std::string& sql);

#endif
