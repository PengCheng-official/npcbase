#include "../include/cli.h"
#include <iostream>
#include <sstream>
#include <vector>

CLI::CLI(TableManager& tableManager) : tableManager_(tableManager) {}

void CLI::run() {
    std::cout << "NPCBase Database CLI" << std::endl;
    std::cout << "Type 'help' for commands. Type 'exit' to quit." << std::endl;
    
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
    } else if (cmd == "create" && args.size() >= 2 && args[0] == "table") {
        handleCreateTable(args);
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
    std::cout << "  insert into <table_name> values (...) - Insert a new record" << std::endl;
    std::cout << "  delete from <table_name> where rid=<page>:<slot> - Delete a record" << std::endl;
    std::cout << "  update <table_name> set ... where rid=<page>:<slot> - Update a record" << std::endl;
    std::cout << "  select from <table_name> where rid=<page>:<slot> - Retrieve a record" << std::endl;
    std::cout << "  vacuum <table_name> - Perform garbage collection" << std::endl;
    std::cout << "  help - Show this help message" << std::endl;
    std::cout << "  exit - Quit the CLI" << std::endl;
}

void CLI::handleCreateTable(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: create table <table_name> (<attr_name> <type> [<length>], ...)" << std::endl;
        return;
    }

    std::string tableName = args[1];
    std::vector<AttrInfo> attrs;

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
    RC rc = tableManager_.createTable(0, tableName.c_str(), attrs.size(), attrs.data());
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
    RC rc = tableManager_.insertRecord(0, tableName.c_str(), data.c_str(), data.length(), rid);
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
        
        RC rc = tableManager_.updateRecord(0, tableName.c_str(), rid, newData.c_str(), newData.length());
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
    if (args.size() < 4 || args[0] != "from" || args[2] != "where" || args[3].substr(0, 4) != "rid=") {
        std::cout << "Usage: select from <table_name> where rid=<page>:<slot>" << std::endl;
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
        
        char* data = nullptr;
        int length = 0;
        RC rc = tableManager_.readRecord(tableName.c_str(), rid, data, length);
        if (rc == RC_OK) {
            std::cout << "Record data: " << std::string(data, length) << std::endl;
            delete[] data;
        } else {
            std::cout << "Error reading record: " << rc << std::endl;
        }
    } catch (...) {
        std::cout << "Invalid RID format" << std::endl;
    }
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
