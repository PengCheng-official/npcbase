//
// Created by 彭诚 on 2025/10/3.
//

#include "../include/cli.h"
#include <iostream>
#include <algorithm>
#include <string>

// 分割命令（按空格分割，忽略多余空格）
std::vector<std::string> CLI::splitCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : command) {
        if (c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// 处理INIT命令（初始化主存和磁盘）
void CLI::handleInit(const std::vector<std::string>& tokens) {
    if (tokens.size() != 5 || tokens[1] != "-mem" || tokens[3] != "-disk") {
        std::cout << "[错误] INIT命令格式错误！正确格式：INIT -mem 主存大小(MB) -disk 磁盘大小(GB)" << std::endl;
        return;
    }
    // 解析主存和磁盘大小
    int mem_size = std::stoi(tokens[2]);
    int disk_size = std::stoi(tokens[4]);
    // 初始化主存和磁盘
    bool mem_ok = mem_manager.initMem(mem_size);
    bool disk_ok = disk_manager.initDisk(disk_size);
    if (mem_ok && disk_ok) {
        is_init = true;
        std::cout << "[初始化] 主存和磁盘初始化完成！" << std::endl;
    }
}

// 处理CREATE TABLE命令
void CLI::handleCreateTable(const std::vector<std::string>& tokens) {
    if (!is_init) {
        std::cout << "[错误] 请先执行INIT命令初始化主存和磁盘！" << std::endl;
        return;
    }
    // 解析表名和字段（格式：CREATE TABLE 表名 (字段1 类型1, 字段2 类型2,...)）
    if (tokens.size() < 4 || tokens[3] != "(" || tokens.back() != ")") {
        std::cout << "[错误] CREATE TABLE格式错误！正确格式：CREATE TABLE 表名 (字段1 类型1, 字段2 类型2,...)" << std::endl;
        return;
    }
    std::string table_name = tokens[2];
    std::vector<std::string> col_names, col_types;
    // 解析字段部分（tokens[4]到tokens[size-2]）
    for (size_t i = 4; i < tokens.size() - 1; ++i) {
        std::string token = tokens[i];
        if (token.back() == ',') {
            token.pop_back();  // 去掉逗号
        }
        // 字段名和类型用空格分割（如"account_id INT"）
        std::vector<std::string> col_parts = splitCommand(token);
        if (col_parts.size() != 2) {
            std::cout << "[错误] 字段格式错误：" << token << std::endl;
            return;
        }
        col_names.push_back(col_parts[0]);
        col_types.push_back(col_parts[1]);
    }
    // 创建表
    table_manager.createTable(table_name, col_names, col_types);
}

// 处理INSERT命令
void CLI::handleInsertRecord(const std::vector<std::string>& tokens) {
    if (!is_init) {
        std::cout << "[错误] 请先执行INIT命令初始化主存和磁盘！" << std::endl;
        return;
    }
    // 解析表名和记录值（格式：INSERT INTO 表名 VALUES (值1, 值2,...)）
    if (tokens.size() < 6 || tokens[3] != "VALUES" || tokens[4] != "(" || tokens.back() != ")") {
        std::cout << "[错误] INSERT格式错误！正确格式：INSERT INTO 表名 VALUES (值1, 值2,...)" << std::endl;
        return;
    }
    std::string table_name = tokens[2];
    Record record;
    // 解析记录值（tokens[5]到tokens[size-2]）
    for (size_t i = 5; i < tokens.size() - 1; ++i) {
        std::string val = tokens[i];
        if (val.back() == ',') {
            val.pop_back();  // 去掉逗号
        }
        record.values.push_back(val);
    }
    // 插入记录
    table_manager.insertRecord(table_name, record);
}

// 处理STATUS命令（打印系统状态）
void CLI::handleStatus() {
    if (!is_init) {
        std::cout << "[错误] 请先执行INIT命令初始化主存和磁盘！" << std::endl;
        return;
    }
    dict.printDict();
    mem_manager.printMemStatus();
    disk_manager.printDiskStatus();
}

// 打印帮助信息
void CLI::printHelp() {
    std::cout << "\n=== npcbase 支持命令 ===" << std::endl;
    std::cout << "1. 初始化主存和磁盘：" << std::endl;
    std::cout << "   INIT -mem <主存大小(MB)> -disk <磁盘大小(GB)>" << std::endl;
    std::cout << "   示例：INIT -mem 16 -disk 1" << std::endl;
    std::cout << "2. 创建表：" << std::endl;
    std::cout << "   CREATE TABLE <表名> (<字段1> <类型1>, <字段2> <类型2>,...)" << std::endl;
    std::cout << "   示例：CREATE TABLE account (account_id INT, balance DECIMAL(10,2))" << std::endl;
    std::cout << "3. 插入记录：" << std::endl;
    std::cout << "   INSERT INTO <表名> VALUES (<值1>, <值2>,...)" << std::endl;
    std::cout << "   示例：INSERT INTO account VALUES (1, 1000.00)" << std::endl;
    std::cout << "4. 查看系统状态：" << std::endl;
    std::cout << "   STATUS" << std::endl;
    std::cout << "5. 查看帮助：" << std::endl;
    std::cout << "   HELP" << std::endl;
    std::cout << "6. 退出：" << std::endl;
    std::cout << "   EXIT" << std::endl;
    std::cout << "=====================\n" << std::endl;
}

// 解析用户命令
void CLI::parseCommand(const std::string& command) {
    std::vector<std::string> tokens = splitCommand(command);
    if (tokens.empty()) return;

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);  // 命令不区分大小写

    if (cmd == "INIT") {
        handleInit(tokens);
    } else if (cmd == "CREATE" && tokens.size() >= 3 && tokens[1] == "TABLE") {
        handleCreateTable(tokens);
    } else if (cmd == "INSERT" && tokens.size() >= 4 && tokens[1] == "INTO") {
        handleInsertRecord(tokens);
    } else if (cmd == "STATUS") {
        handleStatus();
    } else if (cmd == "HELP") {
        printHelp();
    } else if (cmd == "EXIT") {
        std::cout << "退出npcbase..." << std::endl;
        exit(0);
    } else {
        std::cout << "[错误] 未知命令！输入HELP查看支持的命令" << std::endl;
    }
}

// 启动命令行循环
void CLI::run() {
    std::cout << "=== npcbase 数据库（任务1：初始化模块）===" << std::endl;
    std::cout << "输入HELP查看支持的命令，输入EXIT退出" << std::endl;
    std::string command;
    while (true) {
        std::cout << "\nnpcbase> ";
        std::getline(std::cin, command);
        parseCommand(command);
    }
}
