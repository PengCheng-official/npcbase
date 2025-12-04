//
// Created by 彭诚 on 2025/10/9.
//

#include "../include/test.h"
#include "../include/index_manager.h"
#include "../include/sql_ast.h"
#include "../include/sql_plan.h"
#include "../include/sql_physical.h"
#include "../include/npcbase.h"
#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <ctime>

Test::Test(TableManager& tableManager, MemManager& memManager,
           DiskManager& diskManager, DataDict& dataDict, IndexManager& indexManager)
        : tableManager_(tableManager), memManager_(memManager),
          diskManager_(diskManager), dataDict_(dataDict), indexManager_(indexManager) {}

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

RC Test::runTask3() {
    std::cout << "\n===== Starting Task 3 Test: B+ Tree build/insert/update/delete =====" << std::endl;

    const char* tableName = "table3";
    const char* colName = "num";
    const char* indexName = "idx_table3_num";

    // Step 1: Create table (int num)
    TableInfo tbl;
    RC rc = dataDict_.findTable(tableName, tbl);
    if (rc != RC_OK) {
        AttrInfo attr = {"num", INT, 4};
        rc = tableManager_.createTable(1, tableName, 1, &attr);
        if (rc != RC_OK) {
            std::cerr << "Failed to create table '" << tableName << "': " << rc << std::endl;
            return rc;
        }
        dataDict_.findTable(tableName, tbl);
        std::cout << "Created table '" << tableName << "' with single INT column 'num'" << std::endl;
    } else {
        std::cout << "Table '" << tableName << "' already exists, reusing" << std::endl;
    }

    // Step 2: Insert 1000 records (int 0..999)
    int toInsert = 1000;
    int before = tbl.recordCount;
    for (int i = 0; i < toInsert; ++i) {
        int val = i;
        RID rid;
        rc = tableManager_.insertRecord(1, tableName, reinterpret_cast<const char*>(&val), sizeof(int), rid);
        if (rc != RC_OK) { std::cerr << "Insert failed at #" << i << ": " << rc << std::endl; return rc; }
        if ((i + 1) % 200 == 0) std::cout << "  Inserted " << (i + 1) << " records" << std::endl;
    }
    dataDict_.findTable(tableName, tbl);
    std::cout << "Inserted total records: " << (tbl.recordCount - before) << ", current total: " << tbl.recordCount << std::endl;

    // Step 3: Create index on (num) and auto-build B+Tree from existing data
    rc = indexManager_.createIndex(1, indexName, tableName, colName, false);
    if (rc != RC_OK && rc != RC_TABLE_EXISTS) {
        std::cerr << "Failed to create index '" << indexName << "': " << rc << std::endl; return rc;
    } else if (rc == RC_TABLE_EXISTS) {
        std::cout << "Index '" << indexName << "' already exists, reusing" << std::endl;
    } else {
        std::cout << "Index '" << indexName << "' created and built from existing rows" << std::endl;
    }

    // Step 4: Show index file contents
    rc = indexManager_.showIndex(indexName);
    if (rc != RC_OK) { std::cerr << "show index failed: " << rc << std::endl; return rc; }

    // Step 5: Compute keys per page based on page layout
    IndexInfo info; rc = dataDict_.findIndex(indexName, info);
    if (rc != RC_OK) { std::cerr << "findIndex failed: " << rc << std::endl; return rc; }
    int keyLen = info.keyLen; // for INT, should be 4
    int keysPerPage = (int)((BLOCK_SIZE - (int)sizeof(IndexPageHeader)) / (keyLen + 8));
    std::cout << "Computed keys per page: " << keysPerPage
              << " (BLOCK_SIZE=" << BLOCK_SIZE
              << ", header=" << sizeof(IndexPageHeader)
              << ", keyLen=" << keyLen << ", entry=" << (keyLen+8) << ")" << std::endl;

    // Update: delete a few records (first 5 values) and re-show brief summary
    for (int v = 0; v < 5; ++v) {
        RID rid(tbl.firstPage, (SlotNum)v);
        rc = tableManager_.deleteRecord(1, tableName, rid);
        if (rc != RC_OK) { /* skip on error to avoid aborting demo */ }
    }

    std::cout << "After delete 5 records , show index again:" << std::endl;
    indexManager_.showIndex(indexName);

    std::cout << "\n===== Task 3 Test Completed =====" << std::endl;
    return RC_OK;
}

RC Test::runTask4() {
    std::cout << "\n===== Starting Task 4 Test: SQL parse/plan =====" << std::endl;
    // Seed RNG for 3-digit data generation
    std::srand((unsigned int)std::time(nullptr));

    // CREATE TABLE via SQL: two columns (num int, data int)
    std::string createSql = "CREATE TABLE table4 (num int, data int)";
    std::cout << "[SQL] " << createSql << std::endl;
    auto createRes = parseCreateTableSql(createSql);
    if (!createRes.ok) { std::cerr << "Create parse failed: " << createRes.error << std::endl; return RC_INVALID_OP; }
    TableInfo tbl; RC rc = dataDict_.findTable(createRes.create.table.c_str(), tbl);
    if (rc != RC_OK) {
        std::vector<AttrInfo> attrs; attrs.reserve(createRes.create.columns.size());
        for (const auto& cd : createRes.create.columns) {
            AttrInfo ai{}; strncpy(ai.name, cd.name.c_str(), MAX_ATTR_NAME_LEN-1); ai.name[MAX_ATTR_NAME_LEN-1]='\0';
            if (cd.type == "int") { ai.type = INT; ai.length = 4; }
            else if (cd.type == "float") { ai.type = FLOAT; ai.length = 4; }
            else if (cd.type == "string") { ai.type = STRING; ai.length = cd.length>0?cd.length:255; }
            else { std::cerr << "Unsupported type in CREATE: " << cd.type << std::endl; return RC_INVALID_OP; }
            attrs.push_back(ai);
        }
        rc = tableManager_.createTable(1, createRes.create.table.c_str(), (int)attrs.size(), attrs.data());
        if (rc != RC_OK && rc != RC_TABLE_EXISTS) { std::cerr << "Failed to create table4 via SQL: " << rc << std::endl; return rc; }
        dataDict_.findTable(createRes.create.table.c_str(), tbl);
        std::cout << "[CREATE TABLE] Executed: " << createSql << std::endl;
        std::cout << "[CREATE TABLE] Table '" << createRes.create.table << "' ready with " << tbl.attrCount << " column(s)" << std::endl;
    } else {
        std::cout << "[CREATE TABLE] Table '" << createRes.create.table << "' already exists" << std::endl;
    }

    // Helper: pack row according to schema
    auto packRow = [&](const TableInfo& tinfo, const std::vector<std::string>& vals, std::string& out)->bool{
        if ((int)vals.size() != tinfo.attrCount) { std::cerr << "Value count mismatch" << std::endl; return false; }
        out.clear(); out.reserve(64);
        for (int i=0;i<tinfo.attrCount;i++){
            const AttrInfo& a = tinfo.attrs[i];
            const std::string& v = vals[i];
            if (a.type == INT) {
                int x=0; try { x = std::stoi(v); } catch(...) { std::cerr << "Invalid INT literal: " << v << std::endl; return false; }
                out.append(reinterpret_cast<const char*>(&x), sizeof(int));
            } else if (a.type == FLOAT) {
                float f=0.0f; try { f = std::stof(v); } catch(...) { std::cerr << "Invalid FLOAT literal: " << v << std::endl; return false; }
                out.append(reinterpret_cast<const char*>(&f), sizeof(float));
            } else if (a.type == STRING) {
                std::string s=v; if ((int)s.size()>a.length) s.resize(a.length);
                std::string padded = s; padded.resize(a.length, '\0');
                out.append(padded.data(), a.length);
            } else { return false; }
        }
        return true;
    };

    // INSERT via SQL: insert pairs (num, data) with data being a random 3-digit number
    for (int i=0;i<10;i++){
        int dataVal = 100 + std::rand() % 900; // 100..999
        std::string insertSql = std::string("INSERT INTO table4 VALUES (") + std::to_string(i) + ", " + std::to_string(dataVal) + ")";
        std::cout << "[SQL] " << insertSql << std::endl;
        auto insRes = parseInsertSql(insertSql);
        if (!insRes.ok) { std::cerr << "Insert parse failed: " << insRes.error << std::endl; return RC_INVALID_OP; }
        TableInfo ti; dataDict_.findTable(insRes.insert.table.c_str(), ti);
        std::string row; if (!packRow(ti, insRes.insert.values, row)) { return RC_INVALID_OP; }
        RID rid; RC rc = tableManager_.insertRecord(1, ti.tableName, row.data(), (int)row.size(), rid);
        if (rc != RC_OK) { std::cerr << "Insert via SQL failed: " << rc << std::endl; return rc; }
        std::cout << "[INSERT] Executed: " << insertSql << " -> RID " << rid.pageNum << ":" << rid.slotNum << std::endl;
    }
    dataDict_.findTable("table4", tbl);
    std::cout << "[INSERT] Table 'table4' now has " << tbl.recordCount << " records" << std::endl;

    // create index on num to illustrate lookup path
    rc = indexManager_.createIndex(1, "idx_table4_num", "table4", "num", false);
    if (rc == RC_OK) std::cout << "Index created: idx_table4_num" << std::endl; else std::cout << "Index create rc=" << rc << " (may already exist)" << std::endl;

    // Helpers: decode and scan rows from stored pages
    auto decodeRow = [&](const TableInfo& tinfo, const char* buf, int len, int& numOut, int& dataOut){
        numOut = 0; dataOut = 0;
        if (tinfo.attrCount >= 2 && len >= 8) {
            numOut = *reinterpret_cast<const int*>(buf);
            dataOut = *reinterpret_cast<const int*>(buf + sizeof(int));
        }
    };
    auto scanSelectByNum = [&](const TableInfo& tinfo, int qNum){
        for (PageNum p = tinfo.firstPage; p <= tinfo.lastPage; ++p){
            BufferFrame* frame=nullptr; if (memManager_.getPage(tinfo.tableId, p, frame, DATA_SPACE) != RC_OK) continue;
            auto* header = reinterpret_cast<VarPageHeader*>(frame->data);
            int totalSlots = header->recordCount + header->deletedCount;
            for (int s=0; s<totalSlots; ++s){
                auto* slot = reinterpret_cast<RecordSlot*>(frame->data + sizeof(VarPageHeader) + s*sizeof(RecordSlot));
                if (slot->isDeleted) continue;
                int numVal=0, dataVal=0; decodeRow(tinfo, frame->data + slot->offset, slot->length, numVal, dataVal);
                if (numVal == qNum){
                    std::cout << "[SELECT Result] num=" << qNum << " -> data=" << dataVal
                              << " (RID " << p << ":" << s << ")" << std::endl;
                    memManager_.releasePage(tinfo.tableId, p);
                    return;
                }
            }
            memManager_.releasePage(tinfo.tableId, p);
        }
        std::cout << "[SELECT Result] num=" << qNum << " -> not found" << std::endl;
    };
    auto scanSelectAll = [&](const TableInfo& tinfo){
        std::cout << "[SELECT Result] table4 rows:" << std::endl;
        for (PageNum p = tinfo.firstPage; p <= tinfo.lastPage; ++p){
            BufferFrame* frame=nullptr; if (memManager_.getPage(tinfo.tableId, p, frame, DATA_SPACE) != RC_OK) continue;
            auto* header = reinterpret_cast<VarPageHeader*>(frame->data);
            int totalSlots = header->recordCount + header->deletedCount;
            for (int s=0; s<totalSlots; ++s){
                auto* slot = reinterpret_cast<RecordSlot*>(frame->data + sizeof(VarPageHeader) + s*sizeof(RecordSlot));
                if (slot->isDeleted) continue;
                int numVal=0, dataVal=0; decodeRow(tinfo, frame->data + slot->offset, slot->length, numVal, dataVal);
                std::cout << "  num=" << numVal << ", data=" << dataVal
                          << " (RID " << p << ":" << s << ")" << std::endl;
            }
            memManager_.releasePage(tinfo.tableId, p);
        }
    };

    // SELECT via SQL and print result by scanning pages
    int queryNum = 5;
    std::string selectSql = std::string("SELECT data FROM table4 WHERE num = ") + std::to_string(queryNum);
    std::cout << "[SQL] " << selectSql << std::endl;
    auto parseRes = parseSelectSql(selectSql);
    if (!parseRes.ok) { std::cerr << "Parse failed: " << parseRes.error << std::endl; return RC_INVALID_OP; }
    auto lp = buildLogicalPlan(parseRes.select);
    auto opt = optimizeLogicalPlan(lp.plan, dataDict_);
    auto phys = buildPhysicalPlan(opt.optimized, dataDict_, indexManager_);
    std::cout << "[Logical Plan]\n" << printLogicalPlan(lp.plan);
    std::cout << "[Optimized Logical Plan]\n" << printLogicalPlan(opt.optimized);
    std::cout << "[Physical Plan Steps]\n" << printPhysicalPlan(phys);
    scanSelectByNum(tbl, queryNum);

    std::string selectAllSql = "SELECT * FROM table4";
    std::cout << "[SQL] " << selectAllSql << std::endl;
    parseRes = parseSelectSql(selectAllSql);
    auto lp2 = buildLogicalPlan(parseRes.select);
    auto opt2 = optimizeLogicalPlan(lp2.plan, dataDict_);
    auto phys2 = buildPhysicalPlan(opt2.optimized, dataDict_, indexManager_);
    std::cout << "[Logical Plan]\n" << printLogicalPlan(lp2.plan);
    std::cout << "[Optimized Logical Plan]\n" << printLogicalPlan(opt2.optimized);
    std::cout << "[Physical Plan Steps]\n" << printPhysicalPlan(phys2);
    scanSelectAll(tbl);

    std::cout << "===== Task 4 Test Completed =====" << std::endl;
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
