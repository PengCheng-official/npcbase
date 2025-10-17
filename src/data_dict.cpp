#include "../include/data_dict.h"
#include <cstring>
#include <algorithm>
#include <iostream>


DataDict::DataDict(DiskManager &diskManager, MemManager &memManager, LogManager &logManager) :
        diskManager_(diskManager), memManager_(memManager), logManager_(logManager),
        currentLogBlock_(-1)
{
}

RC DataDict::init() {
    // 初始化数据字典，需要从磁盘加载
    tables_.clear();
    nextTableId_ = 1;

    // 从磁盘加载元数据到内存
    BlockNum blockNum = 0;
    char blockData[BLOCK_SIZE];
    long offset = 0;
    int blockOffset = 0;
    while (true) {
        RC rc = diskManager_.readBlock(LOG_TABLE_ID, blockNum, blockData);
        if (rc != RC_OK) {
            break; // 没有更多块了
        }

        // 解析页中存储的TableInfo
        blockOffset = 0;
        while (blockOffset + sizeof(DictPageHeader) <= BLOCK_SIZE) {
            DictPageHeader *header = reinterpret_cast<DictPageHeader *>(blockData + blockOffset);
            if (header->tableCount <= 0 || blockOffset + header->tableCount > BLOCK_SIZE) {
                break; // 无效或超出块大小，停止解析
            }

            TableInfo *table = reinterpret_cast<TableInfo*>(blockOffset + sizeof(DictPageHeader));
            for (int i = 0; i < header->tableCount; i++) {
                if (table->tableId == 0) {
                    break; // 未使用的槽位
                }
                tables_.push_back(*table);
                tableIdToDictPage_[table->tableId] = blockNum;
                if (table->tableId >= nextTableId_) {
                    nextTableId_ = table->tableId + 1; // 更新下一个可用ID
                }
                table++;
                blockOffset += sizeof(TableInfo) + sizeof(DictPageHeader);
                offset += sizeof(TableInfo) + sizeof(DictPageHeader);
            }
        }
        blockOffsets_[blockNum] = blockOffset;
        blockNum++;
    }

    // 如果有表，设置当前表块
    if (offset > 0) {
        currentLogBlock_ = blockNum;
    } else {
        // 分配第一个日志块
        BlockNum newBlock;
        if (diskManager_.allocBlock(DICT_TABLE_ID, newBlock) != RC_OK) {
            return RC_INVALID_BLOCK;
        }
        currentLogBlock_ = newBlock;
        blockOffsets_[currentLogBlock_] = 0;
    }

    return RC_OK;
}

RC DataDict::createTable(TransactionId txId, const char *tableName, int attrCount, const AttrInfo *attrs,
                         TableId &tableId) {
    // 1. 参数有效性检查
    if (tableName == nullptr || strlen(tableName) >= MAX_TABLE_NAME_LEN ||
        attrCount <= 0 || attrCount > MAX_ATTRS_PER_TABLE || attrs == nullptr) {
        return RC_INVALID_ARG;
    }

    // 2. 检查表是否已存在
    TableInfo temp;
    if (findTable(tableName, temp) == RC_OK) {
        return RC_TABLE_EXISTS;
    }

    // 3. 构建新表的元数据
    TableInfo table;
    table.tableId = nextTableId_++;  // 自增生成唯一表ID
    strncpy(table.tableName, tableName, MAX_TABLE_NAME_LEN - 1);
    table.tableName[MAX_TABLE_NAME_LEN - 1] = '\0';  // 确保字符串终止
    table.attrCount = attrCount;
    memcpy(table.attrs, attrs, attrCount * sizeof(AttrInfo));
    table.firstPage = -1;  // 初始无数据页
    table.lastPage = -1;
    table.deletedCount = 0;
    table.recordCount = 0;

    // 4. 创建文件并将元数据写入内存管理器的数据字典缓存区
    RC rc = diskManager_.createTableFile(table.tableId);
    if (rc != RC_OK && rc != RC_FILE_EXISTS) {
        return rc;
    }

    rc = writeToDictCache(table);
    if (rc != RC_OK) {
        // 写入失败回滚内存状态
        nextTableId_--;
        return rc;
    }

    // 5. 内存缓存（供快速查询，非必须但推荐）
    tables_.push_back(table);

    // 6. 记录日志（用于故障恢复）
    logManager_.writeCreateTableLog(txId, table.tableId, tableName, attrCount, attrs);

    tableId = table.tableId;
    return RC_OK;
}

RC DataDict::writeToDictCache(const TableInfo &table) {
    // 在DICT_SPACE分区分配缓冲帧存储元数据
    BufferFrame *frame = nullptr;
    RC rc = memManager_.getPage(DICT_TABLE_ID, currentLogBlock_, frame, DICT_SPACE);
    if (rc != RC_OK) {
        return rc;
    }

    // 是否需要分配新页
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + sizeof(table) > BLOCK_SIZE)) {
        // 分配新日志块
        BlockNum newBlock;
        if (diskManager_.allocBlock(DICT_TABLE_ID, newBlock) != RC_OK) {
            return RC_INVALID_LSN;
        }
        currentLogBlock_ = newBlock;
        blockOffsets_[currentLogBlock_] = 0;
    }

    // 写入内存
    int offset = blockOffsets_[currentLogBlock_];
    TableInfo *tableInfo = reinterpret_cast<TableInfo *>(frame->data + offset);
    *tableInfo = table;

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += sizeof(TableInfo);
    memManager_.markDirty(DICT_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(DICT_TABLE_ID, currentLogBlock_);

    // 记录表ID与元数据页的映射
    tableIdToDictPage_[table.tableId] = currentLogBlock_;

    return RC_OK;
}

RC DataDict::dropTable(TransactionId txId, const char *tableName) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    auto it = std::find_if(tables_.begin(), tables_.end(), [tableName](const TableInfo &table) {
        return strcmp(table.tableName, tableName) == 0;
    });

    if (it == tables_.end()) {
        return RC_TABLE_NOT_FOUND;
    }

    tables_.erase(it);
    return RC_OK;
}

RC DataDict::findTable(const char *tableName, TableInfo &tableInfo) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    // 1. 先查内存缓存
    for (const auto &table : tables_) {
        if (strcmp(table.tableName, tableName) == 0) {
            tableInfo = table;
            return RC_OK;
        }
    }

    // 2. 内存未找到，查缓冲区的元数据页
    for (const auto &[tableId, dictPage] : tableIdToDictPage_) {
        BufferFrame *frame = nullptr;
        RC rc = memManager_.getPage(DICT_TABLE_ID, dictPage, frame, DICT_SPACE);
        if (rc != RC_OK) {
            continue;
        }

        DictPageHeader *pageHeader = reinterpret_cast<DictPageHeader*>(frame->data);
        size_t maxTablePerPage = (BLOCK_SIZE - sizeof(DictPageHeader)) / sizeof(TableInfo);
        int actualCount = std::min(pageHeader->tableCount, (int)maxTablePerPage);

        // 解析页中存储的TableInfo
        TableInfo *table = reinterpret_cast<TableInfo*>(frame->data + sizeof(DictPageHeader));
        for (int i = 0; i < actualCount; i++) {
            if (strcmp(table->tableName, tableName) == 0) {
                tableInfo = *table;
                memManager_.releasePage(DICT_TABLE_ID, dictPage);
                return RC_OK;
            }
            table++;
        }
        memManager_.releasePage(DICT_TABLE_ID, dictPage);
    }

    return RC_TABLE_NOT_FOUND;
}

RC DataDict::findTableById(TableId tableId, TableInfo &tableInfo) {
    for (const auto &table: tables_) {
        if (table.tableId == tableId) {
            tableInfo = table;
            return RC_OK;
        }
    }

    return RC_TABLE_NOT_FOUND;
}

RC DataDict::updateTableInfo(TableId tableId, PageNum lastPage, int recordCount) {
    for (auto &table: tables_) {
        if (table.tableId == tableId) {
            if (table.firstPage == -1) {
                table.firstPage = lastPage;
            }
            table.lastPage = lastPage;
            table.recordCount = recordCount;
            return RC_OK;
        }
    }

    return RC_TABLE_NOT_FOUND;
}

RC DataDict::listTables(std::vector<std::string> &tables) {
    tables.clear();
    for (const auto &table: tables_) {
        tables.push_back(std::string(table.tableName));
    }
    return RC_OK;
}
