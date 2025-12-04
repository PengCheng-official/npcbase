#include "../include/cli.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <limits>
#include "../include/sql_ast.h"
#include "../include/sql_plan.h"
#include "../include/sql_physical.h"

CLI::CLI(TableManager &tableManager, DataDict& dataDict, Test &test, IndexManager &indexManager) : tableManager_(tableManager), dataDict_(dataDict), test_(test), indexManager_(indexManager) {}

void CLI::run() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "\nNPCBase Database CLI" << std::endl;
    std::cout << "Type 'help' for commands.\nType 'exit' to quit." << std::endl;
    
    std::string command;
    while (true) {
        std::cout << "npcbase> ";
        std::getline(std::cin, command);
        
        if (command == "exit") {
            break;
        }
        
        std::vector<std::string> args;
        std::string cmd = parseCommand(command, args);
        
        if (!cmd.empty()) {
            executeCommand(cmd, args);
        }
    }
}

std::string CLI::parseCommand(const std::string& command, std::vector<std::string>& args) {
    std::istringstream iss(command);
    std::string token;
    std::vector<std::string> tokens;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return "";
    }
    
    std::string cmd = tokens[0];
    for (size_t i = 1; i < tokens.size(); i++) {
        args.push_back(tokens[i]);
    }
    
    return cmd;
}

void CLI::executeCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help") {
        printHelp();
    } else if (cmd == "test") {  // 处理test命令
        handleTest(args);
    } else if (cmd == "create" && args.size() >= 2 && args[0] == "table") {
        handleCreateTable(args);
    } else if (cmd == "create" && args.size() >= 2 && args[0] == "index") {
        handleCreateIndex(args);
    } else if (cmd == "show" && args.size() >= 2 && args[0] == "index") {
        handleShowIndex(args);
    } else if (cmd == "insert") {
        handleInsert(args);
    } else if (cmd == "delete") {
        handleDelete(args);
    } else if (cmd == "update") {
        handleUpdate(args);
    } else if (cmd == "select") {
        handleSelect(args);
    } else if (cmd == "vacuum") {
        handleVacuum(args);
    } else {
        std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
    }
}

void CLI::printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  create table <table_name> (<attr_name> <type> [<length>], ...) - Create a new table" << std::endl;
    std::cout << "  create index <index_name> on <table_name>(<column_name>) [unique] - Create a B+tree index" << std::endl;
    std::cout << "  show index <index_name> - Show index page contents" << std::endl;
    std::cout << "  insert into <table_name> values (...) - Insert a new record" << std::endl;
    std::cout << "  delete from <table_name> where rid=<page>:<slot> - Delete a record" << std::endl;
    std::cout << "  update <table_name> set ... where rid=<page>:<slot> - Update a record" << std::endl;
    std::cout << "  select from <table_name> where rid=<page>:<slot> - Retrieve a record" << std::endl;
    std::cout << "  vacuum <table_name> - Perform garbage collection" << std::endl;
    std::cout << "  test <task_idx> - Run a test task" << std::endl;
    std::cout << "  help - Show this help message" << std::endl;
    std::cout << "  exit - Quit the CLI" << std::endl;
}

void CLI::handleTest(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        std::cout << "Usage: test <task_idx>" << std::endl;
        return;
    }

    if (args[0] == "1") {
        test_.runTask1();
    } else if (args[0] == "2") {
        test_.runTask2();
    } else if (args[0] == "3") {
        test_.runTask3();
    } else if (args[0] == "4") {
        test_.runTask4();
    } else {
        std::cout << "Invalid test number. This task is not available" << std::endl;
        return;
    }
}

void CLI::handleCreateTable(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: create table <table_name> (<attr_name> <type> [<length>], ...)" << std::endl;
        return;
    }

    // 辅助函数：清理字符串中的括号和逗号
    auto cleanSymbol = [](const std::string& s) {
        std::string res;
        for (char c : s) {
            if (c != '(' && c != ')' && c != ',') {
                res += c;
            }
        }
        return res;
    };

    std::string tableName = cleanSymbol(args[1]);
    std::vector<AttrInfo> attrs;

    // 解析属性列表（带符号清理）
    for (size_t i = 2; i < args.size(); ) {
        // 清理属性名中的符号（可能包含'(', ')'等）
        std::string attrName = cleanSymbol(args[i]);
        if (attrName.empty()) {  // 跳过空字符串（纯符号的情况）
            i++;
            continue;
        }

        if (i + 1 >= args.size()) {
            std::cout << "Missing type for attribute: " << attrName << std::endl;
            return;
        }

        // 清理类型中的符号（可能包含','等）
        std::string typeStr = cleanSymbol(args[i+1]);
        if (typeStr.empty()) {
            std::cout << "Invalid type for attribute: " << attrName << std::endl;
            return;
        }

        AttrInfo attr;
        strncpy(attr.name, attrName.c_str(), MAX_ATTR_NAME_LEN - 1);
        attr.name[MAX_ATTR_NAME_LEN - 1] = '\0';  // 确保字符串结束符

        // 解析类型
        if (typeStr == "int") {
            attr.type = INT;
            attr.length = 4;
            i += 2;  // 移动到下一个属性名
        } else if (typeStr == "float") {
            attr.type = FLOAT;
            attr.length = 4;
            i += 2;
        } else if (typeStr == "string") {
            attr.type = STRING;
            attr.length = 255;  // 默认长度
            // 检查是否有指定长度
            if (i + 2 < args.size()) {
                std::string lenStr = cleanSymbol(args[i+2]);
                if (!lenStr.empty()) {
                    try {
                        attr.length = std::stoi(lenStr);
                        i += 3;  // 跳过长度参数
                        continue;
                    } catch (...) {
                        std::cout << "Invalid length for string attribute: " << attrName << std::endl;
                        return;
                    }
                }
            }
            i += 2;  // 没有指定长度时移动
        } else {
            std::cout << "Unknown type: " << typeStr << " for attribute: " << attrName << std::endl;
            return;
        }

        attrs.push_back(attr);
    }

    if (attrs.empty()) {
        std::cout << "No attributes specified for table" << std::endl;
        return;
    }

    // TODO: 实际应从事务管理器获取txId
    RC rc = tableManager_.createTable(1, tableName.c_str(), (int)attrs.size(), attrs.data());
    if (rc == RC_OK) {
        std::cout << "Table " << tableName << " created successfully" << std::endl;
    } else if (rc == RC_TABLE_EXISTS) {
        std::cout << "Error: Table " << tableName << " already exists" << std::endl;
    } else {
        std::cout << "Error creating table: " << rc << std::endl;
    }
}

void CLI::handleInsert(const std::vector<std::string>& args) {
    if (args.size() < 3 || args[0] != "into" || args[2] != "values") {
        std::cout << "Usage: insert into <table_name> values (...) - Insert a new record" << std::endl;
        return;
    }
    
    std::string tableName = args[1];
    // 简化实现：假设数据是一个字符串
    std::string data;
    for (size_t i = 3; i < args.size(); i++) {
        if (i > 3) data += " ";
        data += args[i];
    }
    
    RID rid;
    RC rc = tableManager_.insertRecord(0, tableName.c_str(), data.c_str(), (int)data.length(), rid);
    if (rc == RC_OK) {
        std::cout << "Record inserted with RID: " << rid.pageNum << ":" << rid.slotNum << std::endl;
    } else {
        std::cout << "Error inserting record: " << rc << std::endl;
    }
}

void CLI::handleDelete(const std::vector<std::string>& args) {
    if (args.size() < 4 || args[0] != "from" || args[2] != "where" || args[3].substr(0, 4) != "rid=") {
        std::cout << "Usage: delete from <table_name> where rid=<page>:<slot>" << std::endl;
        return;
    }
    
    std::string tableName = args[1];
    std::string ridStr = args[3].substr(4);
    size_t colonPos = ridStr.find(':');
    if (colonPos == std::string::npos) {
        std::cout << "Invalid RID format. Use <page>:<slot>" << std::endl;
        return;
    }
    
    try {
        PageNum pageNum = std::stoi(ridStr.substr(0, colonPos));
        SlotNum slotNum = std::stoi(ridStr.substr(colonPos + 1));
        RID rid(pageNum, slotNum);
        
        RC rc = tableManager_.deleteRecord(0, tableName.c_str(), rid);
        if (rc == RC_OK) {
            std::cout << "Record deleted successfully" << std::endl;
        } else {
            std::cout << "Error deleting record: " << rc << std::endl;
        }
    } catch (...) {
        std::cout << "Invalid RID format" << std::endl;
    }
}

void CLI::handleUpdate(const std::vector<std::string>& args) {
    if (args.size() < 6 || args[0] != "<table_name>" || args[1] != "set" || args[3] != "where" || args[4].substr(0, 4) != "rid=") {
        std::cout << "Usage: update <table_name> set ... where rid=<page>:<slot>" << std::endl;
        return;
    }
    
    std::string tableName = args[0];
    std::string newData = args[2];  // 简化实现
    
    std::string ridStr = args[4].substr(4);
    size_t colonPos = ridStr.find(':');
    if (colonPos == std::string::npos) {
        std::cout << "Invalid RID format. Use <page>:<slot>" << std::endl;
        return;
    }
    
    try {
        PageNum pageNum = std::stoi(ridStr.substr(0, colonPos));
        SlotNum slotNum = std::stoi(ridStr.substr(colonPos + 1));
        RID rid(pageNum, slotNum);
        
        RC rc = tableManager_.updateRecord(0, tableName.c_str(), rid, newData.c_str(), (int)newData.length());
        if (rc == RC_OK) {
            std::cout << "Record updated successfully" << std::endl;
        } else {
            std::cout << "Error updating record: " << rc << std::endl;
        }
    } catch (...) {
        std::cout << "Invalid RID format" << std::endl;
    }
}

void CLI::handleSelect(const std::vector<std::string>& args) {
    // If user entered classic form: select from <table> where rid=<page>:<slot>
    if (args.size() >= 4 && args[0] == "from" && args[2] == "where" && args[3].substr(0, 4) == "rid=") {
        std::string tableName = args[1];
        std::string ridStr = args[3].substr(4);
        size_t colonPos = ridStr.find(':');
        if (colonPos == std::string::npos) {
            std::cout << "Invalid RID format. Use <page>:<slot>" << std::endl;
            return;
        }
        try {
            PageNum pageNum = std::stoi(ridStr.substr(0, colonPos));
            SlotNum slotNum = std::stoi(ridStr.substr(colonPos + 1));
            RID rid(pageNum, slotNum);
            char* data = nullptr; int length = 0;
            RC rc = tableManager_.readRecord(tableName.c_str(), rid, data, length);
            if (rc == RC_OK) { std::cout << "Record data: " << std::string(data, length) << std::endl; delete[] data; }
            else { std::cout << "Error reading record: " << rc << std::endl; }
        } catch (...) { std::cout << "Invalid RID format" << std::endl; }
        return;
    }

    // New full SQL mode: args contains everything after the command token 'select'. Reconstruct SQL string.
    std::string sql = "SELECT"; for (const auto& a : args) { sql += " "; sql += a; }
    auto parseRes = parseSelectSql(sql);
    if (!parseRes.ok) { std::cout << "Parse error: " << parseRes.error << std::endl; return; }

    // Print AST
    std::cout << "[Parse Tree] SELECT columns=";
    for (size_t i=0;i<parseRes.select.columns.size();++i){ if(i) std::cout << ", "; std::cout << parseRes.select.columns[i]; }
    std::cout << " FROM " << parseRes.select.table;
    if (parseRes.select.where.has_value()) {
        const auto& w = parseRes.select.where.value();
        std::cout << " WHERE " << w.column << " " << w.op << " '" << w.literal << "'";
    }
    std::cout << std::endl;

    // Semantic checks: table and columns exist
    TableInfo ti; RC rcTbl = dataDict_.findTable(parseRes.select.table.c_str(), ti);
    if (rcTbl != RC_OK) { std::cout << "Semantic error: table not found: " << parseRes.select.table << std::endl; return; }
    if (!(parseRes.select.columns.size()==1 && parseRes.select.columns[0]=="*")) {
        for (const auto& c : parseRes.select.columns) {
            bool found=false; for(int i=0;i<ti.attrCount;i++){ if (c==ti.attrs[i].name){found=true;break;} }
            if (!found) { std::cout << "Semantic error: column not found: " << c << std::endl; return; }
        }
    }

    // Build logical plan
    auto lp = buildLogicalPlan(parseRes.select);
    if (!lp.ok) { std::cout << "Failed to build logical plan: " << lp.error << std::endl; return; }
    std::cout << "[Logical Plan]\n" << printLogicalPlan(lp.plan);

    // Optimize logical plan
    auto opt = optimizeLogicalPlan(lp.plan, dataDict_);
    if (!opt.ok) { std::cout << "Failed to optimize logical plan: " << opt.error << std::endl; return; }
    std::cout << "[Optimized Logical Plan]\n" << printLogicalPlan(opt.optimized);

    // Physical plan
    auto phys = buildPhysicalPlan(opt.optimized, dataDict_, indexManager_);
    std::cout << "[Physical Plan Steps]\n" << printPhysicalPlan(phys);
}

void CLI::handleVacuum(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cout << "Usage: vacuum <table_name>" << std::endl;
        return;
    }
    
    std::string tableName = args[0];
    RC rc = tableManager_.vacuum(tableName.c_str());
    if (rc == RC_OK) {
        std::cout << "Vacuum completed successfully" << std::endl;
    } else {
        std::cout << "Error during vacuum: " << rc << std::endl;
    }
}

void CLI::handleCreateIndex(const std::vector<std::string> &args) {
    // Syntax: create index <index_name> on <table>(<column>) [unique]
    if (args.size() < 4 || args[2] != "on") {
        std::cout << "Usage: create index <index_name> on <table>(<column>) [unique]" << std::endl;
        return;
    }
    std::string indexName = args[1];
    std::string tableAndCol;
    for (size_t i = 3; i < args.size(); ++i) {
        if (!tableAndCol.empty()) tableAndCol += " ";
        tableAndCol += args[i];
    }
    // parse table and column
    auto lpar = tableAndCol.find('(');
    auto rpar = tableAndCol.find(')');
    if (lpar == std::string::npos || rpar == std::string::npos || rpar <= lpar) {
        std::cout << "Invalid ON clause. Expect <table>(<column>)" << std::endl;
        return;
    }
    std::string tableName = tableAndCol.substr(0, lpar);
    std::string columnName = tableAndCol.substr(lpar + 1, rpar - lpar - 1);
    // trim spaces and commas
    auto trim = [](std::string s){
        size_t b = s.find_first_not_of(" ,");
        size_t e = s.find_last_not_of(" ,");
        if (b == std::string::npos) return std::string();
        return s.substr(b, e - b + 1);
    };
    tableName = trim(tableName);
    columnName = trim(columnName);

    bool unique = tableAndCol.find("unique") != std::string::npos;

    RC rc = indexManager_.createIndex(1, indexName.c_str(), tableName.c_str(), columnName.c_str(), unique);
    if (rc == RC_OK) {
        std::cout << "Index " << indexName << " created on " << tableName << "(" << columnName << ")" << std::endl;
    } else if (rc == RC_TABLE_EXISTS) {
        std::cout << "Index name already exists: " << indexName << std::endl;
    } else if (rc == RC_TABLE_NOT_FOUND) {
        std::cout << "Table not found: " << tableName << std::endl;
    } else if (rc == RC_ATTR_NOT_FOUND) {
        std::cout << "Column not found: " << columnName << std::endl;
    } else {
        std::cout << "Failed to create index. RC=" << rc << std::endl;
    }
}

void CLI::handleShowIndex(const std::vector<std::string> &args) {
    if (args.size() != 2) {
        std::cout << "Usage: show index <index_name>" << std::endl;
        return;
    }
    RC rc = indexManager_.showIndex(args[1].c_str());
    if (rc != RC_OK) {
        std::cout << "Failed to show index. RC=" << rc << std::endl;
    }
}
