//
// Created by 彭诚 on 2025/10/9.
//

#ifndef NPCBASE_TEST_H
#define NPCBASE_TEST_H

#include "table_manager.h"
#include "mem_manager.h"
#include "disk_manager.h"
#include "data_dict.h"
#include <string>
#include <vector>

class Test {
public:
    Test(TableManager& tableManager, MemManager& memManager,
         DiskManager& diskManager, DataDict& dataDict);

    // 执行任务一测试
    RC runTask1();

private:
    TableManager& tableManager_;
    MemManager& memManager_;
    DiskManager& diskManager_;
    DataDict& dataDict_;
    const std::vector<std::string> testTables_ = {
            "test_table_1", "test_table_2", "test_table_3",
            "test_table_4", "test_table_5"
    };

    // 创建测试表
    RC createTestTables();

    // 插入测试数据
    RC insertTestData(const std::string& tableName, int count);

    // 显示现有表
    RC showExistingTables();

    // 展示内存分配
    void showMemoryAllocation();

    // 展示磁盘分配
    void showDiskAllocation();
};

#endif //NPCBASE_TEST_H
