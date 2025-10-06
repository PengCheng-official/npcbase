//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_MEM_MANAGER_H
#define NPCBASE_MEM_MANAGER_H

#include "npcbase.h"
#include "data_dict.h"
#include <vector>

class MemManager {
private:
    std::vector<Block> mem_blocks;   // 主存块列表
    DataDictionary& dict;            // 关联数据字典

public:
    // 构造函数：初始化主存
    MemManager(DataDictionary& d) : dict(d) {}

    // 初始化主存：输入主存大小（MB），划分块
    bool initMem(int mem_size_mb);

    // 打印主存状态（调试用）
    void printMemStatus();
};

#endif //NPCBASE_MEM_MANAGER_H
