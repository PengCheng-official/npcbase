//
// Created by 彭诚 on 2025/10/3.
//

#include "../include/disk_manager.h"
#include <iostream>
#include <cstring>
#include <map>
#include <algorithm>

// 初始化磁盘：输入磁盘大小（GB），划分块并分配系统表初始块
bool DiskManager::initDisk(int disk_size_gb) {
    // 检查磁盘大小是否为块大小的整数倍
    int total_disk_bytes = disk_size_gb * 1024 * 1024 * 1024;
    if (total_disk_bytes % DEFAULT_BLOCK_SIZE != 0) {
        std::cout << "[错误] 磁盘大小（" << disk_size_gb << "GB）需为" << DEFAULT_BLOCK_SIZE/1024 << "KB的整数倍！" << std::endl;
        return false;
    }
    int total_block_num = total_disk_bytes / DEFAULT_BLOCK_SIZE;
    // 1. 分配系统表初始块（SYSTEM类型，table_id=0）
    for (int i = 0; i < SYSTEM_TABLE_INIT_BLOCK_NUM; ++i) {
        std::string block_id = dict.generateDiskBlockId();
        Block block(block_id, "SYSTEM", 0);
        disk_blocks.push_back(block);
        dict.addBlock(block_id, "SYSTEM", 0, DEFAULT_BLOCK_SIZE);
    }
    // 2. 分配剩余空闲块（FREE类型，table_id=0）
    int free_block_num = total_block_num - SYSTEM_TABLE_INIT_BLOCK_NUM;
    for (int i = 0; i < free_block_num; ++i) {
        std::string block_id = dict.generateDiskBlockId();
        Block block(block_id, "FREE", 0);
        disk_blocks.push_back(block);
        dict.addBlock(block_id, "FREE", 0, DEFAULT_BLOCK_SIZE);
    }
    std::cout << "[磁盘管理] 磁盘初始化成功：" << disk_size_gb << "GB → " << total_block_num << "个" << DEFAULT_BLOCK_SIZE/1024 << "KB块" << std::endl;
    std::cout << "  - 系统表块：" << SYSTEM_TABLE_INIT_BLOCK_NUM << "个" << std::endl;
    std::cout << "  - 空闲块：" << free_block_num << "个" << std::endl;
    return true;
}

// 为表分配1个磁盘块（从空闲块中选择）
std::string DiskManager::allocateBlock(int table_id) {
    // 查找第一个空闲磁盘块
    for (auto& block : disk_blocks) {
        if (block.block_type == "FREE") {
            // 更新块属性
            block.block_type = "TABLE_DATA";
            block.table_id = table_id;
            // 数据字典更新块元数据
            dict.updateBlockFreeSpace(block.block_id, block.free_space);
            dict.addBlock(block.block_id, "TABLE_DATA", table_id, block.free_space);
            std::cout << "[磁盘管理] 为table_id=" << table_id << "分配块：" << block.block_id << std::endl;
            return block.block_id;
        }
    }
    std::cout << "[错误] 磁盘无空闲块，无法为table_id=" << table_id << "分配块！" << std::endl;
    return "";
}

// 向磁盘块写入记录（更新块空闲空间和数据）
bool DiskManager::writeRecordToBlock(const std::string& block_id, const Record& record,
                                     const std::vector<std::string>& col_types) {
    // 1. 计算记录大小
    int record_size = record.getSize(col_types);
    // 2. 查找目标块
    for (auto& block : disk_blocks) {
        if (block.block_id == block_id) {
            // 检查空闲空间是否足够
            if (block.free_space < record_size) {
                std::cout << "[错误] 块" << block_id << "空闲空间不足（需" << record_size << "B，现有" << block.free_space << "B）" << std::endl;
                return false;
            }
            // 3. 写入记录（简化：直接将记录值转为字符串存入data）
            std::string record_str;
            for (auto& val : record.values) {
                record_str += val + "|";  // 用"|"分隔字段
            }
            // 写入块数据（从空闲空间起始位置写入）
            int write_pos = DEFAULT_BLOCK_SIZE - block.free_space;
            std::memcpy(&block.data[write_pos], record_str.c_str(), record_str.size());
            // 4. 更新块空闲空间
            block.free_space -= record_size;
            // 5. 同步数据字典
            dict.updateBlockFreeSpace(block_id, block.free_space);
            std::cout << "[磁盘管理] 记录写入块" << block_id << "成功：记录大小=" << record_size << "B，剩余空闲空间=" << block.free_space << "B" << std::endl;
            return true;
        }
    }
    std::cout << "[错误] 未找到块" << block_id << std::endl;
    return false;
}

// 打印磁盘状态（调试用）
void DiskManager::printDiskStatus() {
    std::cout << "\n=== 磁盘状态 ===" << std::endl;
    std::cout << "磁盘块总数：" << disk_blocks.size() << std::endl;
    // 统计各类型块数量
    std::map<std::string, int> type_count;
    for (auto& block : disk_blocks) {
        type_count[block.block_type]++;
    }
    for (auto& [type, cnt] : type_count) {
        std::cout << "  " << type << "块：" << cnt << "个" << std::endl;
    }
    // 打印前10个块详情（避免输出过多）
    std::cout << "前10个块详情：" << std::endl;
    int print_cnt = std::min(10, (int)disk_blocks.size());
    for (int i = 0; i < print_cnt; ++i) {
        auto& block = disk_blocks[i];
        std::cout << "  block_id=" << block.block_id << ", type=" << block.block_type
                  << ", table_id=" << block.table_id << ", free_space=" << block.free_space << "B" << std::endl;
    }
    std::cout << "================\n" << std::endl;
}
