#include "../include/log_manager.h"
#include "../include/npcbase.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>

// 日志文件名常量
const std::string LOG_FILE_NAME = "npcbaseDb.log";
// 日志缓存大小（与块大小一致，便于刷盘）
const int LOG_BUFFER_SIZE = BLOCK_SIZE;

// 构造函数：初始化基础成员，延迟初始化缓存和文件
LogManager::LogManager(DiskManager &diskManager, MemManager &memManager)
        : diskManager_(diskManager),
          memManager_(memManager),
          dbName_(diskManager_.getDbName()),
          currentLSN_(0),
          lastFlushedLSN_(0),
          currentLogBlock_(-1) {}

// 析构函数：确保日志刷新并释放资源
LogManager::~LogManager() {
    // 析构时强制刷新所有日志
    if (currentLSN_ > lastFlushedLSN_) {
        flushLog();
    }
}

// 初始化日志管理器：打开文件、初始化缓存、加载已有日志索引
RC LogManager::init() {
    // 确保日志文件存在
    RC rc = diskManager_.createLogFile();
    if (rc != RC_OK && rc != RC_FILE_EXISTS) {
        return rc;
    }

    // 加载已有日志的LSN索引（用于崩溃恢复）
    BlockNum blockNum = 0;
    char blockData[BLOCK_SIZE];
    long offset = 0;
    int blockOffset = 0;

    while (true) {
        // 尝试读取当前块
        rc = diskManager_.readBlock(LOG_TABLE_ID, blockNum, blockData);
        if (rc != RC_OK) {
            break; // 没有更多块了
        }

        // 解析块中的日志记录
        blockOffset = 0;
        while (blockOffset + sizeof(LogHeader) <= BLOCK_SIZE) {
            LogHeader *header = reinterpret_cast<LogHeader *>(blockData + blockOffset);
            if (header->length <= 0 || blockOffset + header->length > BLOCK_SIZE) {
                break; // 无效日志或超出块大小，停止解析
            }

            // 记录LSN与块和偏移量的映射
            lsnBlockMap_[header->lsn] = {blockNum, blockOffset};

            // 移动到下一条日志
            blockOffset += header->length;
            offset += header->length;

            // 更新当前最大LSN
            if (header->lsn > currentLSN_) {
                currentLSN_ = header->lsn;
            }
        }
        blockOffsets_[blockNum] = blockOffset;  // 记录块内偏移
        blockNum++;
    }

    // 如果有日志，设置当前日志块
    if (offset > 0) {
        currentLogBlock_ = offset / BLOCK_SIZE;
    } else {
        // 分配第一个日志块
        rc = diskManager_.allocBlock(LOG_TABLE_ID, currentLogBlock_);
        if (rc != RC_OK) {
            return rc;
        }
        blockOffsets_[currentLogBlock_] = 0;
    }

    lastFlushedLSN_ = currentLSN_; // 已存在的日志都是已刷盘的
    return RC_OK;
}

// 计算日志记录总长度
int LogManager::calculateLogLength(LogType type, int dataLen, int extraLen) {
    int baseLen = sizeof(LogHeader);
    switch (type) {
        case LOG_BEGIN:
        case LOG_COMMIT:
        case LOG_ABORT:
            return baseLen + sizeof(TxControlLog) - sizeof(LogHeader);
        case LOG_INSERT:
        case LOG_DELETE:
            return baseLen + sizeof(InsertLog) - sizeof(LogHeader) + dataLen;
        case LOG_UPDATE:
            return baseLen + sizeof(UpdateLog) - sizeof(LogHeader) + dataLen + extraLen;
        case LOG_CREATE_TABLE:
            return baseLen + sizeof(CreateTableLog) - sizeof(LogHeader) + extraLen;
        case LOG_DROP_TABLE:
            return baseLen + sizeof(DropTableLog) - sizeof(LogHeader);
        case LOG_ALTER_TABLE:
            return baseLen + extraLen; // 预留
        default:
            return 0;
    }
}

// 从内存管理器读取指定日志块
RC LogManager::readLogBlock(BlockNum blockNum, char *data) {
    BufferFrame *frame = nullptr;
    RC rc = memManager_.getPage(LOG_TABLE_ID, blockNum, frame, LOG_SPACE);
    if (rc != RC_OK) {
        return rc;
    }
    memcpy(data, frame->data, BLOCK_SIZE);
    memManager_.releasePage(LOG_TABLE_ID, blockNum);
    return RC_OK;
}

// 将日志块写入内存管理器
RC LogManager::writeLogBlock(BlockNum blockNum, const char *data) {
    BufferFrame *frame = nullptr;
    RC rc = memManager_.getPage(LOG_TABLE_ID, blockNum, frame, LOG_SPACE);
    if (rc != RC_OK) {
        return rc;
    }
    memcpy(frame->data, data, BLOCK_SIZE);
    memManager_.markDirty(LOG_TABLE_ID, blockNum);
    memManager_.releasePage(LOG_TABLE_ID, blockNum);
    return RC_OK;
}

// 写入事务中止日志
lsn_t LogManager::writeAbortLog(TransactionId txId) {
    // 计算日志长度
    int logLen = calculateLogLength(LOG_ABORT);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN; // 日志记录不能超过块大小
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    TxControlLog *log = reinterpret_cast<TxControlLog *>(frame->data + offset);
    log->header.type = LOG_ABORT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);  // 获取事务前一条日志LSN
    log->header.length = logLen;

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入插入操作日志
lsn_t LogManager::writeInsertLog(TransactionId txId, TableId tableId, const RID &rid,
                                 const char *data, int dataLen) {
    // 计算日志长度
    int logLen = calculateLogLength(LOG_INSERT, dataLen);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    InsertLog *log = reinterpret_cast<InsertLog *>(frame->data + offset);
    log->header.type = LOG_INSERT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->dataLen = dataLen;
    memcpy(log->data, data, dataLen);  // 复制记录数据到柔性数组

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入删除操作日志
lsn_t LogManager::writeDeleteLog(TransactionId txId, TableId tableId, const RID &rid,
                                 const char *data, int dataLen) {
    // 计算日志长度
    int logLen = calculateLogLength(LOG_DELETE, dataLen);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    DeleteLog *log = reinterpret_cast<DeleteLog *>(frame->data + offset);
    log->header.type = LOG_DELETE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->dataLen = dataLen;
    memcpy(log->data, data, dataLen);  // 保存删除的记录数据用于回滚

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入更新操作日志
lsn_t LogManager::writeUpdateLog(TransactionId txId, TableId tableId, const RID &rid,
                                 const char *oldData, int oldLen, const char *newData, int newLen) {
    // 计算日志长度（包含旧数据和新数据）
    int logLen = calculateLogLength(LOG_UPDATE, oldLen, newLen);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    UpdateLog *log = reinterpret_cast<UpdateLog *>(frame->data + offset);
    log->header.type = LOG_UPDATE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->oldDataLen = oldLen;
    log->newDataLen = newLen;
    memcpy(log->data, oldData, oldLen);              // 先存旧数据
    memcpy(log->data + oldLen, newData, newLen);     // 再存新数据

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入创建表日志
lsn_t LogManager::writeCreateTableLog(TransactionId txId, TableId tableId, const char *tableName,
                                      int attrCount, const AttrInfo *attrs) {
    // 计算属性信息总长度
    int attrTotalLen = attrCount * sizeof(AttrInfo);
    // 计算日志总长度
    int logLen = calculateLogLength(LOG_CREATE_TABLE, 0, attrTotalLen);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    CreateTableLog *log = reinterpret_cast<CreateTableLog *>(frame->data + offset);
    log->header.type = LOG_CREATE_TABLE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);
    log->header.length = logLen;
    log->tableId = tableId;
    log->attrCount = attrCount;
    strncpy(log->tableName, tableName, 255);  // 确保不溢出
    log->tableName[255] = '\0';               // 保证字符串终止
    memcpy(log->attrs, attrs, attrTotalLen);   // 复制属性信息

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入删除表日志
lsn_t LogManager::writeDropTableLog(TransactionId txId, TableId tableId, const char *tableName) {
    // 计算日志长度
    int logLen = calculateLogLength(LOG_DROP_TABLE);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    DropTableLog *log = reinterpret_cast<DropTableLog *>(frame->data + offset);
    log->header.type = LOG_DROP_TABLE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = getLastLSN(txId);
    log->header.length = logLen;
    log->tableId = tableId;
    strncpy(log->tableName, tableName, 255);  // 确保不溢出
    log->tableName[255] = '\0';               // 保证字符串终止

    // 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入事务开始日志
lsn_t LogManager::writeBeginLog(TransactionId txId) {
    // 1. 计算日志长度
    int logLen = calculateLogLength(LOG_BEGIN);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    TxControlLog *log = reinterpret_cast<TxControlLog *>(frame->data + offset);
    log->header.type = LOG_BEGIN;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = 0; // 事务第一条日志，无前驱
    log->header.length = logLen;

    // 4. 更新事务日志链跟踪（记录事务最后一条日志LSN）
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    return log->header.lsn;
}

// 写入事务提交日志
lsn_t LogManager::writeCommitLog(TransactionId txId) {
    // 1. 获取事务最后一条日志的LSN（构建日志链）
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_COMMIT);
    if (logLen > BLOCK_SIZE) {
        return RC_INVALID_LSN;
    }

    RC rc = RC_OK;
    if (currentLogBlock_ == -1 || (blockOffsets_[currentLogBlock_] + logLen > BLOCK_SIZE)) {
        rc = allocLogBlock();
    }
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 获取当前日志块的缓冲帧
    BufferFrame *frame = nullptr;
    rc = getCurrentLogBlock(frame);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 构造日志记录
    int offset = blockOffsets_[currentLogBlock_];
    TxControlLog *log = reinterpret_cast<TxControlLog *>(frame->data + offset);
    log->header.type = LOG_COMMIT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN; // 链接到事务的上一条日志
    log->header.length = logLen;

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, offset};

    // 更新块偏移并标记脏页
    blockOffsets_[currentLogBlock_] += logLen;
    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);

    // 8. 提交日志需要立即刷盘
    flushLog(log->header.lsn);

    return log->header.lsn;
}

//// 将日志缓存写入内存管理器的日志分区（仅内存操作，不涉及磁盘块分配）
//RC LogManager::flushBufferToMemManager() {
//    if (bufferPos_ == 0) return RC_OK; // 缓存为空，无需刷新
//
//    BufferFrame* frame = nullptr;
//    RC rc;
//
//    // 如果当前没有日志块，从内存管理器获取一个空闲日志块
//    if (currentLogBlock_ == -1) {
//        PageNum freePage;
//        rc = memManager_.getFreeFrame(frame, freePage, LOG_SPACE);
//        if (rc != RC_OK) {
//            return rc; // 内存日志分区无空闲块
//        }
//        currentLogBlock_ = freePage;
//        // 初始化新日志块（清空数据）
//        memset(frame->data, 0, BLOCK_SIZE);
//    } else {
//        // 获取当前日志块的缓冲帧
//        rc = memManager_.getPage(LOG_TABLE_ID, currentLogBlock_, frame, LOG_SPACE);
//        if (rc != RC_OK) {
//            return rc;
//        }
//    }
//
//    // 计算当前内存块中已使用的空间
//    int usedSpace = 0;
//    LogHeader tempHeader;
//    while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
//        // 从内存块中读取日志头部
//        memcpy(&tempHeader, frame->data + usedSpace, sizeof(LogHeader));
//        if (tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
//            break; // 无效日志或超出块大小，停止计算
//        }
//        usedSpace += tempHeader.length;
//    }
//
//    // 检查当前内存块是否有足够空间
//    if (usedSpace + bufferPos_ > BLOCK_SIZE) {
//        // 当前块空间不足，标记脏页并释放
//        memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_);
//        memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);
//
//        // 从磁盘管理器分配新的日志块
//        BlockNum newBlock;
//        rc = diskManager_.allocBlock(LOG_TABLE_ID, newBlock);
//        if (rc != RC_OK) {
//            return rc;
//        }
//
//        // 从内存管理器获取新的空闲日志块
//        PageNum newPage;
//        rc = memManager_.getFreeFrame(frame, newPage, LOG_SPACE);
//        if (rc != RC_OK) {
//            return rc; // 内存日志分区无空闲块
//        }
//        currentLogBlock_ = newBlock;
//        // 初始化新日志块
//        memset(frame->data, 0, BLOCK_SIZE);
//        usedSpace = 0;
//    }
//
//    // 将缓存中的日志数据复制到内存块
//    memcpy(frame->data + usedSpace, logBuffer_, bufferPos_);
//    memManager_.markDirty(LOG_TABLE_ID, currentLogBlock_); // 标记内存块为脏
//    memManager_.releasePage(LOG_TABLE_ID, currentLogBlock_);
//
//    // 重置缓存
//    memset(logBuffer_, 0, BLOCK_SIZE);
//    bufferPos_ = 0;
//
//    return RC_OK;
//}

RC LogManager::flushLog() {
    // 将内存管理器中LOG_SPACE分区的所有脏页写入磁盘
    RC rc = memManager_.flushSpace(LOG_SPACE);
    if (rc != RC_OK) {
        return rc;
    }
    lastFlushedLSN_ = currentLSN_;

    return RC_OK;
}

// 刷新日志到磁盘
RC LogManager::flushLog(lsn_t lsn) {
    // 如果请求刷新的LSN小于等于最后刷盘的LSN，无需操作
    if (lsn <= lastFlushedLSN_) {
        return RC_OK;
    }

    // 刷新内存中所有日志块到磁盘
    RC rc = memManager_.flushSpace(LOG_SPACE);
    if (rc == RC_OK) {
        lastFlushedLSN_ = currentLSN_;
    }
    return rc;
}

// 读取指定LSN的日志（用于恢复）
RC LogManager::readLog(lsn_t lsn, char *buffer, int &len) {
    if (!lsnBlockMap_.count(lsn)) {
        return RC_LOG_NOT_FOUND;
    }

    auto &blockInfo = lsnBlockMap_[lsn];
    BlockNum blockNum = blockInfo.first;
    int offset = blockInfo.second;

    // 从内存管理器读取日志块
    char blockData[BLOCK_SIZE];
    RC rc = readLogBlock(blockNum, blockData);
    if (rc != RC_OK) {
        return rc;
    }

    // 提取日志头部获取长度
    LogHeader *header = reinterpret_cast<LogHeader *>(blockData + offset);
    len = header->length;

    // 复制日志数据
    if (buffer != nullptr) {
        memcpy(buffer, blockData + offset, len);
    }

    return RC_OK;
}

// 遍历指定事务的完整日志链
RC LogManager::traverseTxLog(TransactionId txId, std::vector<char *> &logChain) {
    lsn_t currentLSN = getLastLSN(txId);
    if (currentLSN == RC_INVALID_LSN) {
        return RC_LOG_READ_ERROR;
    }

    while (currentLSN != 0) {
        int len;
        char *logData = new char[BLOCK_SIZE]; // 最大日志不会超过块大小
        RC rc = readLog(currentLSN, logData, len);
        if (rc != RC_OK) {
            // 清理已分配的内存
            for (char *data: logChain) {
                delete[] data;
            }
            logChain.clear();
            delete[] logData;
            return rc;
        }

        logChain.push_back(logData);

        // 获取前一条日志LSN
        LogHeader *header = reinterpret_cast<LogHeader *>(logData);
        currentLSN = header->prevLSN;
    }

    // 反转日志链，使其按顺序排列
    std::reverse(logChain.begin(), logChain.end());
    return RC_OK;
}

// 获取指定事务的最后一条日志LSN
lsn_t LogManager::getLastLSN(TransactionId txId) const {
    auto it = txLastLSN_.find(txId);
    if (it == txLastLSN_.end()) {
        return RC_INVALID_LSN;
    }
    return it->second;
}

RC LogManager::allocLogBlock() {
    // 尝试刷新日志分区获取空间
    if (memManager_.flushSpace(LOG_SPACE) != RC_OK) {
        return RC_INVALID_LSN;
    }

    // 分配新日志块
    BlockNum newBlock;
    if (diskManager_.allocBlock(LOG_TABLE_ID, newBlock) != RC_OK) {
        return RC_INVALID_LSN;
    }
    currentLogBlock_ = newBlock;
    blockOffsets_[currentLogBlock_] = 0;

    return RC_OK;
}

RC LogManager::getCurrentLogBlock(BufferFrame *&frame) {
    RC rc = memManager_.getPage(LOG_TABLE_ID, currentLogBlock_, frame, LOG_SPACE);
    if (rc != RC_OK) {
        return RC_INVALID_LSN;
    }
    return RC_OK;
}
