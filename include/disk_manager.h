//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_DISK_MANAGER_H
#define NPCBASE_DISK_MANAGER_H

#include "npcbase.h"
#include "data_dict.h"
#include <vector>

class DiskManager {
private:
    std::vector<Block> disk_blocks;  // 磁盘块列表
    DataDictionary& dict;            // 关联数据字典

public:
    // 构造函数：初始化磁盘
    DiskManager(DataDictionary& d) : dict(d) {}

    // 初始化磁盘：输入磁盘大小（GB），划分块并分配系统表初始块
    bool initDisk(int disk_size_gb);

    // 为表分配1个磁盘块（从空闲块中选择）
    std::string allocateBlock(int table_id);

    // 向磁盘块写入记录（更新块空闲空间和数据）
    bool writeRecordToBlock(const std::string& block_id, const Record& record,
                            const std::vector<std::string>& col_types);

    // 打印磁盘状态（调试用）
    void printDiskStatus();
};

#endif //NPCBASE_DISK_MANAGER_H
