//
// Created by 彭诚 on 2025/10/9.
//

#include "../include/test.h"
#include <iostream>

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


RC Test::runTask2() {
    std::cout << "\n===== Starting Task 2 Test =====" << std::endl;
    std::cout << "Testing memory management: partitions and content verification" << std::endl;

    // 1. 初始内存状态检查
    std::cout << "\n[Step 1] Initial memory partition status:" << std::endl;
    showMemoryPartitions();
    showAllPartitionDetails();

    // 2. 生成各类内存数据
    std::cout << "\n[Step 2] Generating memory data..." << std::endl;

    // 创建表操作会更新数据字典（DICT_SPACE）
    RC rc = createTestTables();
    if (rc != RC_OK) {
        std::cerr << "Failed to create test tables: " << rc << std::endl;
        return rc;
    }

    // 插入数据会使用数据缓存（DATA_SPACE）
    rc = insertTestData("test_table_1", 1000);
    if (rc != RC_OK) {
        std::cerr << "Failed to insert data: " << rc << std::endl;
        return rc;
    }

    // 3. 生成数据后的内存状态检查
    std::cout << "\n[Step 3] Memory status after data generation:" << std::endl;
    showAllPartitionDetails();

    std::cout << "\n===== Task 2 Test Completed =====" << std::endl;
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

void Test::showMemoryPartitions() {
    std::cout << "Memory partitions overview:" << std::endl;
    std::cout << "  PLAN_SPACE: " << memManager_.planFrames_ << " frames ("
              << memManager_.planCacheSize_ << " bytes)" << std::endl;
    std::cout << "  DICT_SPACE: " << memManager_.dictFrames_ << " frames ("
              << memManager_.dictCacheSize_ << " bytes)" << std::endl;
    std::cout << "  DATA_SPACE: " << memManager_.dataFrames_ << " frames ("
              << memManager_.dataCacheSize_ << " bytes)" << std::endl;
    std::cout << "  LOG_SPACE: " << memManager_.logFrames_ << " frames ("
              << memManager_.logCacheSize_ << " bytes)" << std::endl;
}

void Test::showPartitionDetails(MemSpaceType type, const std::string &name) {
    std::cout << "\nDetailed info for " << name << ":" << std::endl;
    int usedFrames = 0;
    int dirtyFrames = 0;

    for (const auto& frame : memManager_.frames_) {
        if (frame.spaceType == type && frame.pageNum != -1) {
            usedFrames++;
            if (frame.isDirty) dirtyFrames++;

            std::cout << "  Frame (table: " << frame.tableId
                      << ", page: " << frame.pageNum
                      << ", pin: " << frame.pinCount
                      << ", dirty: " << (frame.isDirty ? "yes" : "no")
                      << ", ref: " << (frame.refBit ? "yes" : "no") << ")" << std::endl;
        }
    }

    std::cout << "  Summary: " << usedFrames << " used frames ("
              << dirtyFrames << " dirty) out of "
              << (type == PLAN_SPACE ? memManager_.planFrames_ :
                  type == DICT_SPACE ? memManager_.dictFrames_ :
                  type == DATA_SPACE ? memManager_.dataFrames_ :
                  memManager_.logFrames_) << " total frames" << std::endl;
}

void Test::showAllPartitionDetails() {
    showPartitionDetails(PLAN_SPACE, "Access Plans");
    showPartitionDetails(DICT_SPACE, "Data Dictionary");
    showPartitionDetails(DATA_SPACE, "Data Cache");
    showPartitionDetails(LOG_SPACE, "Log Cache");
}
