//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_DATA_DICT_H
#define NPCBASE_DATA_DICT_H

#include "npcbase.h"
#include <vector>
#include <string>

class DataDictionary {
private:
    std::vector<DictTable> tables;   // 所有表元数据
    std::vector<DictBlock> blocks;   // 所有块元数据
    int next_table_id = 1;           // 下一个表ID（从1开始）
    int next_mem_block_idx = 1;      // 下一个主存块索引
    int next_disk_block_idx = 1;     // 下一个磁盘块索引

public:
    // 生成主存块ID
    std::string generateMemBlockId();

    // 生成磁盘块ID
    std::string generateDiskBlockId();

    // 添加表元数据
    int addTable(const std::string& table_name,
                 const std::vector<std::string>& col_names,
                 const std::vector<std::string>& col_types);

    // 添加块元数据
    void addBlock(const std::string& block_id, const std::string& block_type,
                  int table_id, int free_space);

    // 根据表名查询表元数据
    DictTable* getTableByName(const std::string& table_name);

    // 根据表ID查询该表的所有块元数据
    std::vector<DictBlock> getBlocksByTableId(int table_id);

    // 更新块元数据（空闲空间）
    bool updateBlockFreeSpace(const std::string& block_id, int new_free_space);

    // 打印数据字典（调试用）
    void printDict();
};

#endif //NPCBASE_DATA_DICT_H
