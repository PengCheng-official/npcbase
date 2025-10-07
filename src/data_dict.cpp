#include "../include/data_dict.h"
#include <cstring>
#include <algorithm>

RC DataDict::init() {
    // 初始化数据字典，实际实现中可能需要从磁盘加载
    tables_.clear();
    nextTableId_ = 1;
    return RC_OK;
}

RC DataDict::createTable(TransactionId txId, const char* tableName, int attrCount, const AttrInfo* attrs, TableId& tableId) {
    // 检查参数有效性
    if (tableName == nullptr || strlen(tableName) >= MAX_TABLE_NAME_LEN ||
        attrCount <= 0 || attrCount > MAX_ATTRS_PER_TABLE || attrs == nullptr) {
        return RC_INVALID_ARG;
    }

    // 检查是否已存在同名表
    TableInfo temp;
    if (findTable(tableName, temp) == RC_OK) {
        return RC_TABLE_EXISTS;
    }

    // 创建新表信息
    TableInfo table;
    table.tableId = nextTableId_++;
    strncpy(table.tableName, tableName, MAX_TABLE_NAME_LEN - 1);
    table.attrCount = attrCount;
    memcpy(table.attrs, attrs, attrCount * sizeof(AttrInfo));
    table.firstPage = -1;
    table.lastPage = -1;
    table.deletedCount = 0;
    table.recordCount = 0;

    tables_.push_back(table);
    tableId = table.tableId;

    // 记录创建表的日志（需要定义相应的日志类型）
     logManager_->writeCreateTableLog(txId, tableId, tableName, attrCount, attrs);

    return RC_OK;
}

RC DataDict::dropTable(TransactionId txId, const char *tableName) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    auto it = std::find_if(tables_.begin(), tables_.end(), [tableName](const TableInfo& table) {
        return strcmp(table.tableName, tableName) == 0;
    });

    if (it == tables_.end()) {
        return RC_TABLE_NOT_FOUND;
    }

    tables_.erase(it);
    return RC_OK;
}

RC DataDict::findTable(const char* tableName, TableInfo& tableInfo) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    for (const auto& table : tables_) {
        if (strcmp(table.tableName, tableName) == 0) {
            tableInfo = table;
            return RC_OK;
        }
    }

    return RC_TABLE_NOT_FOUND;
}

RC DataDict::findTableById(TableId tableId, TableInfo& tableInfo) {
    for (const auto& table : tables_) {
        if (table.tableId == tableId) {
            tableInfo = table;
            return RC_OK;
        }
    }

    return RC_TABLE_NOT_FOUND;
}

RC DataDict::updateTableInfo(TableId tableId, PageNum lastPage, int recordCount) {
    for (auto& table : tables_) {
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

RC DataDict::listTables(std::vector<std::string>& tables) {
    tables.clear();
    for (const auto& table : tables_) {
        tables.push_back(std::string(table.tableName));
    }
    return RC_OK;
}
