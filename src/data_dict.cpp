#include "../include/data_dict.h"
#include <cstring>
#include <algorithm>


DataDict::DataDict(MemManager &memManager, LogManager &logManager) :
        memManager_(memManager), logManager_(logManager) {
}

RC DataDict::init() {
    // 初始化数据字典，实际实现中可能需要从磁盘加载
    tables_.clear();
    nextTableId_ = 1;

    // 从磁盘加载元数据到内存
    PageNum dictPage = 0;
    BufferFrame *frame = nullptr;
    while (true) {
        RC rc = memManager_.getPage(DICT_TABLE_ID, dictPage, frame, DICT_SPACE);
        if (rc == RC_PAGE_NOT_FOUND) {
            break; // 无更多元数据页
        }
        if (rc != RC_OK) {
            return rc; // 加载失败
        }

        DictPageHeader *pageHeader = reinterpret_cast<DictPageHeader*>(frame->data);
        size_t maxTablePerPage = (BLOCK_SIZE - sizeof(DictPageHeader)) / sizeof(TableInfo);

        // 解析页中存储的TableInfo
        int actualCount = std::min(pageHeader->tableCount, (int)maxTablePerPage);
        if (actualCount < 0 || actualCount > (int)maxTablePerPage) {
            actualCount = 0; // 无效计数时视为空页
        }

        TableInfo *table = reinterpret_cast<TableInfo*>(frame->data + sizeof(DictPageHeader));
        for (int i = 0; i < actualCount; i++) {
            if (table->tableId == 0) {
                break; // 未使用的槽位
            }
            tables_.push_back(*table);
            tableIdToDictPage_[table->tableId] = dictPage;
            if (table->tableId >= nextTableId_) {
                nextTableId_ = table->tableId + 1; // 更新下一个可用ID
            }
            table++;
        }

        memManager_.releasePage(DICT_TABLE_ID, dictPage);
        dictPage++;
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

    // 4. 将元数据写入内存管理器的数据字典缓存区
    RC rc = writeToDictCache(table);
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
    PageNum dictPage; // 元数据页编号（可动态分配或固定）
    RC rc = memManager_.getFreeFrame(frame, dictPage, DICT_SPACE);
    if (rc != RC_OK) {
        return rc; // 分配失败
    }

    // 计算当前页可存储的最大表数量
    size_t maxTablePerPage = (BLOCK_SIZE - sizeof(DictPageHeader)) / sizeof(TableInfo);
    DictPageHeader *header = reinterpret_cast<DictPageHeader*>(frame->data);

    // 初始化新页的页头（如果是新分配的页）
    if (header->tableCount < 0 || header->tableCount > maxTablePerPage) {
        header->tableCount = 0; // 首次使用的页初始化计数
    }

    // 检查页是否还有空间
    if (header->tableCount >= maxTablePerPage) {
        memManager_.releasePage(DICT_TABLE_ID, dictPage);
        return RC_OUT_OF_MEMORY; // 页已满，需要处理
    }

    // 计算新表项的存储位置
    TableInfo *tableEntry = reinterpret_cast<TableInfo*>(
            frame->data + sizeof(DictPageHeader) + header->tableCount * sizeof(TableInfo)
    );

    // 复制表信息到页中
    *tableEntry = table;
    header->tableCount++; // 关键修复：更新页内表数量

    // 标记为脏页并设置pinCount
    frame->isDirty = true;
    frame->pinCount = 1;

    // 记录表ID与元数据页的映射
    tableIdToDictPage_[table.tableId] = dictPage;

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

    // 2. 内存未找到，查磁盘元数据页
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
