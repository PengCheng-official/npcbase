//
// Created by 彭诚 on 2025/10/3.
//

#include "../include/table_manager.h"
#include <iostream>

// 创建表：生成表元数据 + 分配初始磁盘块
bool TableManager::createTable(const std::string& table_name,
                               const std::vector<std::string>& col_names,
                               const std::vector<std::string>& col_types) {
    // 1. 检查表名是否已存在
    if (dict.getTableByName(table_name) != nullptr) {
        std::cout << "[错误] 表" << table_name << "已存在！" << std::endl;
        return false;
    }
    // 2. 数据字典添加表元数据
    int table_id = dict.addTable(table_name, col_names, col_types);
    // 3. 分配初始磁盘块
    std::string first_block_id = disk_manager.allocateBlock(table_id);
    if (first_block_id.empty()) {
        return false;
    }
    // 4. 更新表元数据（块数量、首块ID）
    DictTable* table = dict.getTableByName(table_name);
    if (table != nullptr) {
        table->block_count = 1;
        table->first_block_id = first_block_id;
    }
    return true;
}

// 插入记录：查找合适块 → 写入记录（无合适块则分配新块）
bool TableManager::insertRecord(const std::string& table_name, const Record& record) {
    // 1. 查找表元数据
    DictTable* table = dict.getTableByName(table_name);
    if (table == nullptr) {
        std::cout << "[错误] 表" << table_name << "不存在！" << std::endl;
        return false;
    }
    // 2. 检查记录字段数与表字段数匹配
    if (record.values.size() != table->col_names.size()) {
        std::cout << "[错误] 记录字段数（" << record.values.size() << "）与表字段数（" << table->col_names.size() << "）不匹配！" << std::endl;
        return false;
    }
    // 3. 计算记录大小
    int record_size = record.getSize(table->col_types);
    if (record_size > DEFAULT_BLOCK_SIZE) {
        std::cout << "[错误] 记录大小（" << record_size << "B）超过块大小（" << DEFAULT_BLOCK_SIZE << "B）！" << std::endl;
        return false;
    }
    // 4. 查找该表的所有块，选择第一个空闲空间足够的块
    std::vector<DictBlock> table_blocks = dict.getBlocksByTableId(table->table_id);
    std::string target_block_id = "";
    for (auto& block : table_blocks) {
        if (block.free_space >= record_size) {
            target_block_id = block.block_id;
            break;
        }
    }
    // 5. 无合适块 → 分配新块
    if (target_block_id.empty()) {
        target_block_id = disk_manager.allocateBlock(table->table_id);
        if (target_block_id.empty()) {
            return false;
        }
        // 更新表元数据（块数量）
        table->block_count++;
    }
    // 6. 写入记录到目标块
    return disk_manager.writeRecordToBlock(target_block_id, record, table->col_types);
}
