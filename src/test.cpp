//
// Created by 彭诚 on 2025/10/9.
//

#include "../include/test.h"
#include <iostream>
#include <cstring>

Test::Test(TableManager& tableManager, MemManager& memManager,
           DiskManager& diskManager, DataDict& dataDict)
        : tableManager_(tableManager), memManager_(memManager),
          diskManager_(diskManager), dataDict_(dataDict) {}

RC Test::runTask1() {
    std::cout << "\n===== Starting Task 1 Test =====" << std::endl;

    // 展示现有数据
    std::cout << "\n[Step 1] Existing tables before test:" << std::endl;
    RC rc = showExistingTables();
    if (rc != RC_OK) {
        std::cerr << "Failed to show existing tables: " << rc << std::endl;
        return rc;
    }

    // 创建测试表
    std::cout << "\n[Step 2] Creating test tables..." << std::endl;
    rc = createTestTables();
    if (rc != RC_OK) {
        std::cerr << "Failed to create test tables: " << rc << std::endl;
        return rc;
    }

    // 插入测试数据
    std::cout << "\n[Step 3] Inserting 1000 records into each table..." << std::endl;
    for (const auto& tableName : testTables_) {
        rc = insertTestData(tableName, 1000);
        if (rc != RC_OK) {
            std::cerr << "Failed to insert data into " << tableName << std::endl;
            return rc;
        }
    }

    // 展示内存分配
    std::cout << "\n[Step 4] Memory allocation status:" << std::endl;
    showMemoryAllocation();

    // 刷新内存到磁盘
    std::cout << "\n[Step 5] Flushing memory to disk..." << std::endl;
    rc = memManager_.flushAllPages();
    if (rc != RC_OK) {
        std::cerr << "Failed to flush memory: " << rc << std::endl;
        return rc;
    }

    // 展示磁盘分配
    std::cout << "\n[Step 6] Disk allocation status after flush:" << std::endl;
    showDiskAllocation();

    std::cout << "\n===== Task 1 Test Completed =====" << std::endl;
    return RC_OK;
}

RC Test::createTestTables() {
    // 定义表结构：仅包含一个int类型的id字段
    AttrInfo attr = {"num", INT, sizeof(int)};

    for (const auto& tableName : testTables_) {
        // 检查表是否已存在
        TableInfo tableInfo;
        RC rc = dataDict_.findTable(tableName.c_str(), tableInfo);
        if (rc == RC_OK) {
            std::cout << "Table " << tableName << " already exists, skipping" << std::endl;
            continue;
        }

        // 创建新表, 事务相关 txId 暂时忽略
        rc = tableManager_.createTable(0, tableName.c_str(), 1, &attr);
        if (rc != RC_OK) {
            std::cerr << "Failed to create table " << tableName << " (error: " << rc << ")" << std::endl;
            return rc;
        }
        std::cout << "Created table: " << tableName << std::endl;
    }
    return RC_OK;
}

RC Test::insertTestData(const std::string& tableName, int count) {
    for (int i = 0; i < count; ++i) {
        // 简单数据：仅包含一个整数
        int data = i;
        RID rid;

        RC rc = tableManager_.insertRecord(1, tableName.c_str(),
                                           reinterpret_cast<const char*>(&data),
                                           sizeof(int), rid);
        if (rc != RC_OK) {
            std::cerr << "Insert failed at record " << i << " (error: " << rc << ")" << std::endl;
            return rc;
        }

        // 每100条记录显示一次进度
        if ((i + 1) % 100 == 0) {
            std::cout << "Inserted " << (i + 1) << " records into " << tableName << std::endl;
        }
    }
    return RC_OK;
}

RC Test::showExistingTables() {
    int existsCount = 0;
    for (const auto& tableName : testTables_) {
        TableInfo tableInfo;
        RC rc = dataDict_.findTable(tableName.c_str(), tableInfo);
        if (rc == RC_OK) {
            std::cout << "Table " << tableName << " exists with "
                      << tableInfo.recordCount << " records" << std::endl;
            existsCount++;
        }
        else if (rc == RC_TABLE_NOT_FOUND) break;
        else return rc;
    }
    if (existsCount == 0) {
        std::cout << "No test tables exist" << std::endl;
    }
    return RC_OK;
}

void Test::showMemoryAllocation() {
    // 统计每个测试表使用的内存页
    for (const auto& tableName : testTables_) {
        TableInfo tableInfo;
        if (dataDict_.findTable(tableName.c_str(), tableInfo) == RC_OK) {
            std::cout << tableName << " memory usage: " << std::endl;
            std::cout << "  Total records: " << tableInfo.recordCount << std::endl;
            std::cout << "  Pages allocated: " << (tableInfo.lastPage - tableInfo.firstPage + 1) << std::endl;
        }
    }
}

void Test::showDiskAllocation() {
    // 展示每个表在磁盘上的块分配
    for (const auto& tableName : testTables_) {
        TableInfo tableInfo;
        if (dataDict_.findTable(tableName.c_str(), tableInfo) == RC_OK) {
            TableFileHeader header;
            if (diskManager_.readTableFileHeader(tableInfo.tableId, header) == RC_OK) {
                std::cout << tableName << " disk usage: " << std::endl;
                std::cout << "  Total blocks: " << header.totalBlocks << std::endl;
                std::cout << "  Used blocks: " << header.usedBlocks << std::endl;
            }
        }
    }
}