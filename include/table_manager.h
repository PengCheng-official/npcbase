//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_TABLE_MANAGER_H
#define NPCBASE_TABLE_MANAGER_H

#include "npcbase.h"
#include "data_dict.h"
#include "disk_manager.h"

class TableManager {
private:
    DataDictionary& dict;         // 关联数据字典
    DiskManager& disk_manager;    // 关联磁盘管理

public:
    TableManager(DataDictionary& d, DiskManager& dm) : dict(d), disk_manager(dm) {}

    // 创建表：生成表元数据 + 分配初始磁盘块
    bool createTable(const std::string& table_name,
                     const std::vector<std::string>& col_names,
                     const std::vector<std::string>& col_types);

    // 插入记录：查找合适块 → 写入记录（无合适块则分配新块）
    bool insertRecord(const std::string& table_name, const Record& record);
};

#endif //NPCBASE_TABLE_MANAGER_H
