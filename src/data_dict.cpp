#include "../include/data_dict.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <unordered_map>


DataDict::DataDict(DiskManager &diskManager, MemManager &memManager, LogManager &logManager) :
        diskManager_(diskManager), memManager_(memManager), logManager_(logManager),
        currentLogBlock_(-1)
{
}

RC DataDict::init() {
    // 初始化数据字典（从磁盘的DICT_TABLE_ID顺序读取TableInfo，简化实现）
    tables_.clear();
    indexes_.clear();
    tableIdToDictPage_.clear();
    blockOffsets_.clear();
    indexMetaBlockOffsets_.clear();
    nextTableId_ = 1;
    nextIndexId_ = 10000;

    // 1) 加载表元数据
    BlockNum blockNum = 0;
    char blockData[BLOCK_SIZE];
    bool anyRead = false;

    while (true) {
        RC rc = diskManager_.readBlock(DICT_TABLE_ID, blockNum, blockData);
        if (rc != RC_OK) {
            break; // 没有更多块
        }
        anyRead = true;

        int offset = 0;
        while (offset + (int)sizeof(TableInfo) <= BLOCK_SIZE) {
            auto *table = reinterpret_cast<TableInfo *>(blockData + offset);
            if (table->tableId != 0) {
                tables_.push_back(*table);
                tableIdToDictPage_[table->tableId] = blockNum;
                if (table->tableId >= nextTableId_) nextTableId_ = table->tableId + 1;
            }
            offset += sizeof(TableInfo);
        }
        blockOffsets_[blockNum] = (offset % BLOCK_SIZE);
        blockNum++;
    }

    if (!anyRead) {
        // 分配第一个元数据块
        BlockNum newBlock;
        RC rc = diskManager_.allocBlock(DICT_TABLE_ID, newBlock);
        if (rc != RC_OK) return rc;
        currentLogBlock_ = newBlock;
        blockOffsets_[currentLogBlock_] = 0;
    } else {
        // 使用最后一个块（若满则再分配一个）
        BlockNum lastBlock = blockNum - 1;
        int lastOffset = blockOffsets_[lastBlock];
        if (lastOffset + (int)sizeof(TableInfo) > BLOCK_SIZE) {
            BlockNum newBlock;
            RC rc = diskManager_.allocBlock(DICT_TABLE_ID, newBlock);
            if (rc != RC_OK) return rc;
            currentLogBlock_ = newBlock;
            blockOffsets_[currentLogBlock_] = 0;
        } else {
            currentLogBlock_ = lastBlock;
        }
    }

    // 2) 加载索引元数据（sys_indexes）
    BlockNum idxBlock = 0;
    bool anyIndexRead = false;
    std::unordered_map<std::string, IndexInfo> lastByName;

    while (true) {
        RC rc = diskManager_.readBlock(INDEX_META_TABLE_ID, idxBlock, blockData);
        if (rc != RC_OK) break;
        anyIndexRead = true;
        int offset = 0;
        while (offset + (int)sizeof(IndexInfo) <= BLOCK_SIZE) {
            auto* idx = reinterpret_cast<IndexInfo*>(blockData + offset);
            if (idx->indexId != 0 && idx->indexName[0] != '\0') {
                lastByName[std::string(idx->indexName)] = *idx; // 覆盖为最新
                if (idx->indexId >= nextIndexId_) nextIndexId_ = idx->indexId + 1;
            }
            offset += sizeof(IndexInfo);
        }
        indexMetaBlockOffsets_[idxBlock] = (offset % BLOCK_SIZE);
        idxBlock++;
    }

    indexes_.clear();
    indexes_.reserve(lastByName.size());
    for (auto& kv : lastByName) indexes_.push_back(kv.second);

    if (!anyIndexRead) {
        // 初始化索引元数据块
        BlockNum newBlock;
        RC rc = diskManager_.allocBlock(INDEX_META_TABLE_ID, newBlock);
        if (rc != RC_OK) return rc;
        indexMetaCurrentBlock_ = newBlock;
        indexMetaBlockOffsets_[indexMetaCurrentBlock_] = 0;
    } else {
        BlockNum last = (idxBlock == 0) ? 0 : (idxBlock - 1);
        int lastOffset = indexMetaBlockOffsets_[last];
        if (lastOffset + (int)sizeof(IndexInfo) > BLOCK_SIZE) {
            BlockNum newBlock;
            RC rc = diskManager_.allocBlock(INDEX_META_TABLE_ID, newBlock);
            if (rc != RC_OK) return rc;
            indexMetaCurrentBlock_ = newBlock;
            indexMetaBlockOffsets_[indexMetaCurrentBlock_] = 0;
        } else {
            indexMetaCurrentBlock_ = last;
        }
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
    // 确保有可用的当前块
    if (currentLogBlock_ == -1 || blockOffsets_.find(currentLogBlock_) == blockOffsets_.end()) {
        BlockNum newBlock;
        RC arc = diskManager_.allocBlock(DICT_TABLE_ID, newBlock);
        if (arc != RC_OK) return arc;
        currentLogBlock_ = newBlock;
        blockOffsets_[currentLogBlock_] = 0;
    }

    // 如果不足以写入一个TableInfo，分配新块
    if (blockOffsets_[currentLogBlock_] + (int)sizeof(TableInfo) > BLOCK_SIZE) {
        BlockNum newBlock;
        RC arc = diskManager_.allocBlock(DICT_TABLE_ID, newBlock);
        if (arc != RC_OK) return arc;
        currentLogBlock_ = newBlock;
        blockOffsets_[currentLogBlock_] = 0;
    }

    BufferFrame *frame = nullptr;
    RC rc = memManager_.getPage(DICT_TABLE_ID, currentLogBlock_, frame, DICT_SPACE);
    if (rc != RC_OK) {
        return rc;
    }

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

    // 仅从内存缓存查找（初始化时已加载）
    for (const auto &table : tables_) {
        if (strcmp(table.tableName, tableName) == 0) {
            tableInfo = table;
            return RC_OK;
        }
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

// ======== 索引元数据（内存持久化简化版本）========
RC DataDict::createIndexMetadata(TransactionId txId, const char *indexName, const char *tableName,
                                 const char *columnName, bool unique, IndexInfo &outIndex) {
    if (!indexName || !tableName || !columnName) return RC_INVALID_ARG;

    // 检查同名索引
    for (const auto& idx : indexes_) {
        if (strcmp(idx.indexName, indexName) == 0) return RC_TABLE_EXISTS; // 复用错误码表示已存在
    }

    // 表和列合法性
    TableInfo tableInfo;
    RC rc = findTable(tableName, tableInfo);
    if (rc != RC_OK) return rc;

    AttrType keyType = INT; int keyLen = 4; bool found = false;
    for (int i = 0; i < tableInfo.attrCount; ++i) {
        if (strcmp(tableInfo.attrs[i].name, columnName) == 0) {
            keyType = tableInfo.attrs[i].type;
            keyLen = (tableInfo.attrs[i].type == STRING) ? tableInfo.attrs[i].length : 4;
            found = true; break;
        }
    }
    if (!found) return RC_ATTR_NOT_FOUND;

    // 分配索引ID并创建索引文件
    TableId indexId = nextIndexId_++;
    rc = diskManager_.createTableFile(indexId);
    if (rc != RC_OK && rc != RC_FILE_EXISTS) return rc;

    // 组装元数据
    IndexInfo info{};
    info.indexId = indexId;
    strncpy(info.indexName, indexName, MAX_TABLE_NAME_LEN - 1);
    info.indexName[MAX_TABLE_NAME_LEN - 1] = '\0';
    info.tableId = tableInfo.tableId;
    strncpy(info.tableName, tableInfo.tableName, MAX_TABLE_NAME_LEN - 1);
    info.tableName[MAX_TABLE_NAME_LEN - 1] = '\0';
    strncpy(info.columnName, columnName, MAX_ATTR_NAME_LEN - 1);
    info.columnName[MAX_ATTR_NAME_LEN - 1] = '\0';
    info.keyType = keyType;
    info.keyLen = keyLen;
    info.rootPage = -1;
    info.unique = unique;
    info.height = 0;
    info.totalPages = 0;
    info.totalKeys = 0;

    indexes_.push_back(info);
    outIndex = info;

    // 追加写入到索引元数据文件
    appendIndexMeta(info);
    return RC_OK;
}

RC DataDict::findIndex(const char *indexName, IndexInfo &outIndex) {
    if (!indexName) return RC_INVALID_ARG;
    for (const auto& idx : indexes_) {
        if (strcmp(idx.indexName, indexName) == 0) { outIndex = idx; return RC_OK; }
    }
    return RC_TABLE_NOT_FOUND;
}

RC DataDict::listIndexesForTable(TableId tableId, std::vector<IndexInfo> &outIndexes) {
    outIndexes.clear();
    for (const auto& idx : indexes_) if (idx.tableId == tableId) outIndexes.push_back(idx);
    return RC_OK;
}

RC DataDict::updateIndexInfo(const IndexInfo &info) {
    for (auto& idx : indexes_) {
        if (idx.indexId == info.indexId) { idx = info; appendIndexMeta(info); return RC_OK; }
    }
    // 未找到则追加
    indexes_.push_back(info);
    appendIndexMeta(info);
    return RC_OK;
}

RC DataDict::appendIndexMeta(const IndexInfo &info) {
    // 确保当前块有效
    if (indexMetaCurrentBlock_ == -1 || indexMetaBlockOffsets_.find(indexMetaCurrentBlock_) == indexMetaBlockOffsets_.end()) {
        BlockNum newBlock; RC rc = diskManager_.allocBlock(INDEX_META_TABLE_ID, newBlock); if (rc != RC_OK) return rc;
        indexMetaCurrentBlock_ = newBlock; indexMetaBlockOffsets_[indexMetaCurrentBlock_] = 0;
    }
    // 空间不足则分配新块
    if (indexMetaBlockOffsets_[indexMetaCurrentBlock_] + (int)sizeof(IndexInfo) > BLOCK_SIZE) {
        BlockNum newBlock; RC rc = diskManager_.allocBlock(INDEX_META_TABLE_ID, newBlock); if (rc != RC_OK) return rc;
        indexMetaCurrentBlock_ = newBlock; indexMetaBlockOffsets_[indexMetaCurrentBlock_] = 0;
    }

    BufferFrame* frame = nullptr;
    RC rc = memManager_.getPage(INDEX_META_TABLE_ID, indexMetaCurrentBlock_, frame, DICT_SPACE);
    if (rc != RC_OK) return rc;
    int offset = indexMetaBlockOffsets_[indexMetaCurrentBlock_];
    auto* dst = reinterpret_cast<IndexInfo*>(frame->data + offset);
    *dst = info;

    indexMetaBlockOffsets_[indexMetaCurrentBlock_] += sizeof(IndexInfo);
    memManager_.markDirty(INDEX_META_TABLE_ID, indexMetaCurrentBlock_);
    memManager_.releasePage(INDEX_META_TABLE_ID, indexMetaCurrentBlock_);
    return RC_OK;
}
