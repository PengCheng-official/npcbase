//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_CLI_H
#define NPCBASE_CLI_H

#include "data_dict.h"
#include "mem_manager.h"
#include "disk_manager.h"
#include "table_manager.h"
#include <string>
#include <vector>

class CLI {
private:
    DataDictionary dict;         // 数据字典
    MemManager mem_manager;      // 主存管理
    DiskManager disk_manager;    // 磁盘管理
    TableManager table_manager;  // 表管理
    bool is_init = false;        // 是否已完成主存/磁盘初始化

    // 分割命令（按空格分割，忽略多余空格）
    std::vector<std::string> splitCommand(const std::string& command);

    // 处理INIT命令（初始化主存和磁盘）
    void handleInit(const std::vector<std::string>& tokens);

    // 处理CREATE TABLE命令
    void handleCreateTable(const std::vector<std::string>& tokens);

    // 处理INSERT命令
    void handleInsertRecord(const std::vector<std::string>& tokens);

    // 处理STATUS命令（打印系统状态）
    void handleStatus();

    // 打印帮助信息
    void printHelp();

public:
    CLI() : mem_manager(dict), disk_manager(dict), table_manager(dict, disk_manager) {}

    // 解析用户命令
    void parseCommand(const std::string& command);

    // 启动命令行循环
    void run();
};

#endif //NPCBASE_CLI_H
