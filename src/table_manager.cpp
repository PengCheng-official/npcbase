#include "../include/table_manager.h"
#include <cstring>
#include <iostream>

TableManager::TableManager(DataDict &dataDict, DiskManager &diskManager, MemManager &memManager, LogManager &logManager)
        : dataDict_(dataDict), memManager_(memManager), diskManager_(diskManager), logManager_(logManager) {}

RC TableManager::createTable(TransactionId txId, const char *tableName, int attrCount, const AttrInfo *attrs) {
    if (tableName == nullptr || attrCount <= 0 || attrCount > MAX_ATTRS_PER_TABLE || attrs == nullptr) {
        return RC_INVALID_ARG;
    }

    // 检查属性是否有效
    for (int i = 0; i < attrCount; i++) {
        if (attrs[i].type == STRING && (attrs[i].length <= 0 || attrs[i].length > MAX_RECORD_LEN)) {
            return RC_INVALID_ARG;
        }
    }

    // 创建表并添加到数据字典
    TableId tableId;
    RC rc = dataDict_.createTable(txId, tableName, attrCount, attrs, tableId);
    if (rc != RC_OK) {
        return rc;
    }

    return RC_OK;
}

RC TableManager::dropTable(const char *tableName) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    // 获取表信息
    TableInfo tableInfo;
    RC rc = dataDict_.findTable(tableName, tableInfo);
    if (rc != RC_OK) {
        return rc;
    }

    // 释放表的所有数据块（简化实现）
    // 实际实现中应该遍历所有页面并释放

    // 从数据字典中删除表
    return dataDict_.dropTable(0, tableName);
}

RC TableManager::insertRecord(TransactionId txId, const char *tableName, const char *data, int length, RID &rid) {
    if (tableName == nullptr || data == nullptr || length <= 0 || length > MAX_RECORD_LEN) {
        return RC_INVALID_ARG;
    }

    // 获取表信息
    TableInfo tableInfo;
    RC rc = dataDict_.findTable(tableName, tableInfo);
    if (rc != RC_OK) {
        return rc;
    }

    // 计算记录所需空间
    int requiredSpace = calculateRecordSpace(length);
    if (requiredSpace > BLOCK_SIZE - sizeof(VarPageHeader)) {
        return RC_RECORD_TOO_LONG;
    }

    // 查找适合插入的页面
    PageNum pageNum;
    BufferFrame *frame = nullptr;
    rc = findPageForInsert(tableInfo, requiredSpace, pageNum, frame);
    if (rc != RC_OK) {
        return rc;
    }

    // 在页面中查找空闲槽位
    SlotNum slotNum;
    rc = findFreeSlot(frame->data, length, slotNum);
    if (rc != RC_OK) {
        memManager_.releasePage(tableInfo.tableId, pageNum);
        return rc;
    }

    // 写入记录数据
    VarPageHeader *header = reinterpret_cast<VarPageHeader *>(frame->data);
    RecordSlot *slot = reinterpret_cast<RecordSlot *>(frame->data + sizeof(VarPageHeader) +
                                                      slotNum * sizeof(RecordSlot));

    slot->offset = header->freeOffset;
    slot->length = length;
    slot->isDeleted = false;

    // 复制记录数据
    memcpy(frame->data + header->freeOffset, data, length);

    // 更新页面头
    header->freeOffset += length;
    header->recordCount++;
    tableInfo.recordCount++;

    // 更新表信息
    dataDict_.updateTableInfo(tableInfo.tableId, pageNum, tableInfo.recordCount);

    // 标记页面为脏页
    memManager_.markDirty(tableInfo.tableId, pageNum);

    // 记录插入日志
    logManager_.writeInsertLog(txId, LOG_TABLE_ID, RID(pageNum, slotNum), data, length);

    // 设置返回的RID
    rid = RID(pageNum, slotNum);

    // 释放页面
    memManager_.releasePage(tableInfo.tableId, pageNum);

    return RC_OK;
}

RC TableManager::deleteRecord(TransactionId txId, const char *tableName, const RID &rid) {
    if (tableName == nullptr || rid.pageNum < 0 || rid.slotNum < 0) {
        return RC_INVALID_ARG;
    }

    // 获取表信息
    TableInfo tableInfo;
    RC rc = dataDict_.findTable(tableName, tableInfo);
    if (rc != RC_OK) {
        return rc;
    }

    // 获取页面
    BufferFrame *frame = nullptr;
    rc = memManager_.getPage(tableInfo.tableId, rid.pageNum, frame, DATA_SPACE);
    if (rc != RC_OK) {
        return rc;
    }

    // 检查槽位是否有效
    VarPageHeader *header = reinterpret_cast<VarPageHeader *>(frame->data);
    if (rid.slotNum >= header->recordCount + header->deletedCount) {
        memManager_.releasePage(tableInfo.tableId, rid.pageNum);
        return RC_SLOT_NOT_FOUND;
    }

    // 读取要删除的数据用于日志
    RecordSlot *slot = reinterpret_cast<RecordSlot *>(frame->data + sizeof(VarPageHeader) +
                                                      rid.slotNum * sizeof(RecordSlot));
    if (slot->isDeleted) {
        memManager_.releasePage(tableInfo.tableId, rid.pageNum);
        return RC_INVALID_OP;
    }

    char *data = frame->data + slot->offset;
    int dataLen = slot->length;

    // 记录删除日志
    logManager_.writeDeleteLog(txId, tableInfo.tableId, rid, data, dataLen);

    // 标记记录为删除
    slot->isDeleted = true;
    header->deletedCount++;

    // 将空闲空间添加到空闲列表（优化插入）
    if (header->freeListCount < 16) {
        header->freeList[header->freeListCount++] = rid.slotNum;
    }

    // 标记页面为脏页
    memManager_.markDirty(tableInfo.tableId, rid.pageNum);

    // 释放页面
    memManager_.releasePage(tableInfo.tableId, rid.pageNum);

    return RC_OK;
}

RC TableManager::updateRecord(TransactionId txId, const char *tableName, const RID &rid, const char *newData,
                              int newLength) {
    if (tableName == nullptr || newData == nullptr || newLength <= 0 || newLength > MAX_RECORD_LEN) {
        return RC_INVALID_ARG;
    }

    // 先删除旧记录
    RC rc = deleteRecord(0, tableName, rid);
    if (rc != RC_OK) {
        return rc;
    }

    // 插入新记录（简化实现，实际可能需要更高效的方式）
    RID newRid;
    rc = insertRecord(0, tableName, newData, newLength, newRid);
    return rc;
}

RC TableManager::readRecord(const char *tableName, const RID &rid, char *&data, int &length) {
    if (tableName == nullptr || rid.pageNum < 0 || rid.slotNum < 0) {
        return RC_INVALID_ARG;
    }

    // 获取表信息
    TableInfo tableInfo;
    RC rc = dataDict_.findTable(tableName, tableInfo);
    if (rc != RC_OK) {
        return rc;
    }

    // 获取页面
    BufferFrame *frame = nullptr;
    rc = memManager_.getPage(tableInfo.tableId, rid.pageNum, frame, DATA_SPACE);
    if (rc != RC_OK) {
        return rc;
    }

    // 检查槽位是否有效
    VarPageHeader *header = reinterpret_cast<VarPageHeader *>(frame->data);
    if (rid.slotNum >= header->recordCount + header->deletedCount) {
        memManager_.releasePage(tableInfo.tableId, rid.pageNum);
        return RC_SLOT_NOT_FOUND;
    }

    // 读取记录数据
    RecordSlot *slot = reinterpret_cast<RecordSlot *>(frame->data + sizeof(VarPageHeader) +
                                                      rid.slotNum * sizeof(RecordSlot));
    if (slot->isDeleted) {
        memManager_.releasePage(tableInfo.tableId, rid.pageNum);
        return RC_SLOT_NOT_FOUND;
    }

    length = slot->length;
    data = new char[length];
    memcpy(data, frame->data + slot->offset, length);

    // 释放页面
    memManager_.releasePage(tableInfo.tableId, rid.pageNum);

    return RC_OK;
}

RC TableManager::vacuum(const char *tableName) {
    if (tableName == nullptr) {
        return RC_INVALID_ARG;
    }

    // 获取表信息
    TableInfo tableInfo;
    RC rc = dataDict_.findTable(tableName, tableInfo);
    if (rc != RC_OK) {
        return rc;
    }

    if (tableInfo.firstPage == -1) {
        return RC_OK;  // 空表无需清理
    }

    // 遍历所有页面执行垃圾回收（简化实现）
    PageNum currentPage = tableInfo.firstPage;
    while (currentPage != -1) {
        // 获取页面
        BufferFrame *frame = nullptr;
        rc = memManager_.getPage(tableInfo.tableId, currentPage, frame, DATA_SPACE);
        if (rc != RC_OK) {
            return rc;
        }

        VarPageHeader *header = reinterpret_cast<VarPageHeader *>(frame->data);

        // 如果没有删除的记录，跳过此页
        if (header->deletedCount == 0) {
            memManager_.releasePage(tableInfo.tableId, currentPage);
            currentPage++;  // 简化：假设页面连续
            continue;
        }

        // 整理页面：移动有效记录，更新槽信息
        int newFreeOffset = sizeof(VarPageHeader) + (header->recordCount) * sizeof(RecordSlot);
        int validCount = 0;

        // 重新组织记录
        for (int i = 0; i < header->recordCount + header->deletedCount; i++) {
            RecordSlot *slot = reinterpret_cast<RecordSlot *>(frame->data + sizeof(VarPageHeader) +
                                                              i * sizeof(RecordSlot));

            if (!slot->isDeleted) {
                // 移动有效记录到新位置
                memmove(frame->data + newFreeOffset, frame->data + slot->offset, slot->length);

                // 更新槽信息
                slot->offset = newFreeOffset;
                newFreeOffset += slot->length;
                validCount++;
            }
        }

        // 更新页面头
        header->freeOffset = newFreeOffset;
        header->recordCount = validCount;
        header->deletedCount = 0;
        header->freeListCount = 0;
        memset(header->freeList, 0, sizeof(header->freeList));

        // 标记页面为脏页
        memManager_.markDirty(tableInfo.tableId, currentPage);

        // 释放页面
        memManager_.releasePage(tableInfo.tableId, currentPage);

        currentPage++;  // 简化：假设页面连续
    }

    // 更新表的记录计数
    dataDict_.updateTableInfo(tableInfo.tableId, tableInfo.lastPage, tableInfo.recordCount - tableInfo.deletedCount);

    return RC_OK;
}

void TableManager::initNewPage(char *pageData, PageNum pageNum) {
    VarPageHeader *header = reinterpret_cast<VarPageHeader *>(pageData);
    header->pageNum = pageNum;
    header->freeOffset = sizeof(VarPageHeader); // 空闲空间从页面头之后开始
    header->recordCount = 0;
    header->deletedCount = 0;
    memset(header->freeList, -1, sizeof(header->freeList)); // 初始化空闲槽位为-1（无效）
    header->freeListCount = 0;
}

RC TableManager::findPageForInsert(const TableInfo &tableInfo, int length, PageNum &pageNum, BufferFrame *&frame) {
    // 检查是否有现有页面可以容纳新记录
    if (tableInfo.lastPage != -1) {
        RC rc = memManager_.getPage(tableInfo.tableId, tableInfo.lastPage, frame, DATA_SPACE);
        if (rc == RC_OK) {
            VarPageHeader *header = reinterpret_cast<VarPageHeader *>(frame->data);
            int remainingSpace = BLOCK_SIZE - header->freeOffset;  // 计算页面剩余可用空间

            if (remainingSpace >= length + 2) {  // 预留2字节作为记录长度标记
                pageNum = tableInfo.lastPage;
                return RC_OK;
            }

            memManager_.releasePage(tableInfo.tableId, tableInfo.lastPage);
        }
    }

    // 需要分配新页面
    BlockNum newBlockNum;
    RC rc = diskManager_.allocBlock(tableInfo.tableId, newBlockNum);
    if (rc != RC_OK) {
        return rc;
    }

    pageNum = newBlockNum;  // 简化：页号直接使用块号

    // 获取新页面并初始化
    rc = memManager_.getPage(tableInfo.tableId, pageNum, frame, DATA_SPACE);
    if (rc != RC_OK) {
        diskManager_.freeBlock(tableInfo.tableId, newBlockNum);
        return rc;
    }

    // 初始化新页面元数据
    initNewPage(frame->data, pageNum);
    frame->isDirty = true;

//    // 更新缓冲帧信息
//    frame->tableId = tableInfo.tableId;
//    frame->pageNum = pageNum;
//    frame->isDirty = true; // 新页面需要写入磁盘
//    frame->pinCount = 1;  // 增加引用计数

//    // 将新页面写入磁盘（持久化初始化数据）
//    rc = diskManager_.writeBlock(newBlockNum, frame->data);
//    if (rc != RC_OK) {
//        memManager_.releasePage(tableInfo.tableId, pageNum);
//        diskManager_.freeBlock(newBlockNum);
//        return rc;
//    }

    // 更新表信息
    dataDict_.updateTableInfo(tableInfo.tableId, pageNum, tableInfo.recordCount);

    return RC_OK;
}

RC TableManager::findFreeSlot(char *pageData, int length, SlotNum &slotNum) {
    VarPageHeader *header = reinterpret_cast<VarPageHeader *>(pageData);

    // 优先使用空闲列表中的槽位（优化插入）
    for (int i = 0; i < header->freeListCount; i++) {
        SlotNum s = header->freeList[i];
        RecordSlot *slot = reinterpret_cast<RecordSlot *>(pageData + sizeof(VarPageHeader) + s * sizeof(RecordSlot));

        if (slot->isDeleted && slot->length >= length) {
            // 使用此槽位
            slotNum = s;

            // 从空闲列表中移除
            for (int j = i; j < header->freeListCount - 1; j++) {
                header->freeList[j] = header->freeList[j + 1];
            }
            header->freeListCount--;

            return RC_OK;
        }
    }

    // 没有合适的空闲槽位，使用新槽位
    slotNum = header->recordCount + header->deletedCount;
    return RC_OK;
}

int TableManager::calculateRecordSpace(int length) {
    // 记录数据长度 + 槽信息长度
    return length + sizeof(RecordSlot);
}
