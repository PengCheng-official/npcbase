//
// Created by 彭诚 on 2025/10/3.
//

#include "../include/mem_manager.h"
#include <iostream>

// 初始化主存：输入主存大小（MB），划分块
bool MemManager::initMem(int mem_size_mb) {
    // 检查主存大小是否为块大小的整数倍
    int total_mem_bytes = mem_size_mb * 1024 * 1024;
    if (total_mem_bytes % DEFAULT_BLOCK_SIZE != 0) {
        std::cout << "[错误] 主存大小（" << mem_size_mb << "MB）需为" << DEFAULT_BLOCK_SIZE/1024 << "KB的整数倍！" << std::endl;
        return false;
    }
    int block_num = total_mem_bytes / DEFAULT_BLOCK_SIZE;
    // 创建主存块（初始均为空闲块，table_id=0）
    for (int i = 0; i < block_num; ++i) {
        std::string block_id = dict.generateMemBlockId();
        Block block(block_id, "FREE", 0);
        mem_blocks.push_back(block);
        // 数据字典添加主存块元数据
        dict.addBlock(block_id, "FREE", 0, DEFAULT_BLOCK_SIZE);
    }
    std::cout << "[主存管理] 主存初始化成功：" << mem_size_mb << "MB → " << block_num << "个" << DEFAULT_BLOCK_SIZE/1024 << "KB块" << std::endl;
    return true;
}

// 打印主存状态（调试用）
void MemManager::printMemStatus() {
    std::cout << "\n=== 主存状态 ===" << std::endl;
    std::cout << "主存块总数：" << mem_blocks.size() << std::endl;
    for (auto& block : mem_blocks) {
        std::cout << "  block_id=" << block.block_id << ", type=" << block.block_type
                  << ", table_id=" << block.table_id << ", free_space=" << block.free_space << "B" << std::endl;
    }
    std::cout << "================\n" << std::endl;
}