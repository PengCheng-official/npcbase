//
// Created by 彭诚 on 2025/10/3.
//


#include "../include/data_dict.h"
#include <iostream>
#include <cstdio>

// 生成主存块ID（M_001, M_002...）
std::string DataDictionary::generateMemBlockId() {
    char buf[20];
    std::sprintf(buf, "%s%03d", MEM_BLOCK_PREFIX.c_str(), next_mem_block_idx++);
    return buf;
}

// 生成磁盘块ID（D_001, D_002...）
std::string DataDictionary::generateDiskBlockId() {
    char buf[20];
    std::sprintf(buf, "%s%03d", DISK_BLOCK_PREFIX.c_str(), next_disk_block_idx++);
    return buf;
}

// 添加表元数据
int DataDictionary::addTable(const std::string& table_name,
                             const std::vector<std::string>& col_names,
                             const std::vector<std::string>& col_types) {
    DictTable table;
    table.table_id = next_table_id++;
    table.table_name = table_name;
    table.block_count = 0;
    table.first_block_id = "";
    table.col_names = col_names;
    table.col_types = col_types;
    tables.push_back(table);
    std::cout << "[数据字典] 表创建成功：table_id=" << table.table_id << ", table_name=" << table_name << std::endl;
    return table.table_id;
}

// 添加块元数据
void DataDictionary::addBlock(const std::string& block_id, const std::string& block_type,
                              int table_id, int free_space) {
    DictBlock block;
    block.block_id = block_id;
    block.block_type = block_type;
    block.table_id = table_id;
    block.free_space = free_space;
    blocks.push_back(block);
}

// 根据表名查询表元数据
DictTable* DataDictionary::getTableByName(const std::string& table_name) {
    for (auto& table : tables) {
        if (table.table_name == table_name) {
            return &table;
        }
    }
    return nullptr;
}

// 根据表ID查询该表的所有块元数据
std::vector<DictBlock> DataDictionary::getBlocksByTableId(int table_id) {
    std::vector<DictBlock> res;
    for (auto& block : blocks) {
        if (block.table_id == table_id && block.block_type == "TABLE_DATA") {
            res.push_back(block);
        }
    }
    return res;
}

// 更新块元数据（空闲空间）
bool DataDictionary::updateBlockFreeSpace(const std::string& block_id, int new_free_space) {
    for (auto& block : blocks) {
        if (block.block_id == block_id) {
            block.free_space = new_free_space;
            return true;
        }
    }
    return false;
}

// 打印数据字典（调试用）
void DataDictionary::printDict() {
    std::cout << "\n=== 数据字典状态 ===" << std::endl;
    // 打印表信息
    std::cout << "1. 表列表：" << std::endl;
    for (auto& table : tables) {
        std::cout << "   table_id=" << table.table_id << ", name=" << table.table_name
                  << ", block_count=" << table.block_count << ", first_block=" << table.first_block_id << std::endl;
        std::cout << "     字段：";
        for (size_t i = 0; i < table.col_names.size(); ++i) {
            std::cout << table.col_names[i] << "(" << table.col_types[i] << ") ";
        }
        std::cout << std::endl;
    }
    // 打印块信息
    std::cout << "2. 块列表：" << std::endl;
    for (auto& block : blocks) {
        std::cout << "   block_id=" << block.block_id << ", type=" << block.block_type
                  << ", table_id=" << block.table_id << ", free_space=" << block.free_space << "B" << std::endl;
    }
    std::cout << "===================\n" << std::endl;
}
