#include "../include/log_manager.h"
#include "../include/npcbase.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>

// 日志文件名常量
const std::string LOG_FILE_NAME = "redbase.log";
// 日志缓存大小（与块大小一致，便于刷盘）
const int LOG_BUFFER_SIZE = BLOCK_SIZE;

// 构造函数：初始化基础成员，延迟初始化缓存和文件
LogManager::LogManager(DiskManager &diskManager, MemManager &memManager)
        : diskManager_(diskManager),
          memManager_(memManager),
          currentLSN_(0),
          lastFlushedLSN_(0),
          logBuffer_(nullptr),
          bufferSize_(LOG_BUFFER_SIZE),
          bufferPos_(0),
          currentLogBlock_(-1),
          logFileName_(LOG_FILE_NAME) {}

// 析构函数：确保日志刷新并释放资源
LogManager::~LogManager() {
    // 析构时强制刷新所有日志
    if (currentLSN_ > lastFlushedLSN_) {
        flushLog();
    }

    // 关闭日志文件
    if (logFile_.is_open()) {
        logFile_.close();
    }

    // 释放缓存
    if (logBuffer_) {
        delete[] logBuffer_;
        logBuffer_ = nullptr;
    }
}

// 初始化日志管理器：打开文件、初始化缓存、加载已有日志索引
RC LogManager::init() {
    // 打开日志文件（读写模式，不存在则创建）
    logFile_.open(logFileName_, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFileName_ << std::endl;
        return RC_LOG_FILE_ERROR;
    }

    // 初始化日志缓存
    logBuffer_ = new char[bufferSize_];
    memset(logBuffer_, 0, bufferSize_);

    // 加载已有日志的LSN索引（用于崩溃恢复）
    logFile_.seekg(0, std::ios::beg);
    LogHeader header;
    long offset = 0;
    BlockNum blockNum = 0;
    int blockOffset = 0;

    while (logFile_.read(reinterpret_cast<char*>(&header), sizeof(LogHeader))) {
        // 计算当前日志所在的块和偏移
        blockNum = offset / BLOCK_SIZE;
        blockOffset = offset % BLOCK_SIZE;

        // 记录LSN与块和偏移量的映射
        lsnBlockMap_[header.lsn] = {blockNum, blockOffset};

        // 移动到下一条日志
        offset += header.length;
        logFile_.seekg(offset, std::ios::beg);

        // 更新当前最大LSN
        if (header.lsn > currentLSN_) {
            currentLSN_ = header.lsn;
        }
    }

    // 如果有日志，设置当前日志块
    if (offset > 0) {
        currentLogBlock_ = offset / BLOCK_SIZE;
    } else {
        // 分配第一个日志块
        if (diskManager_.allocBlock(currentLogBlock_) != RC_OK) {
            return RC_OUT_OF_DISK;
        }
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

// 将日志缓存写入内存管理器，由其负责后续刷盘
RC LogManager::flushBufferToMemManager() {
    if (bufferPos_ == 0) return RC_OK; // 缓存为空，无需刷新

    BufferFrame* frame = nullptr;
    RC rc;

    // 如果当前没有日志块，分配一个新的
    if (currentLogBlock_ == -1) {
        if (diskManager_.allocBlock(currentLogBlock_) != RC_OK) {
            return RC_OUT_OF_DISK;
        }
    }

    // 获取当前日志块的缓冲帧
    rc = memManager_.getPage(-1, currentLogBlock_, frame, LOG_SPACE);
    if (rc != RC_OK) {
        return rc;
    }

    // 计算当前块中已使用的空间
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(currentLogBlock_ * BLOCK_SIZE, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(currentLogBlock_ * BLOCK_SIZE + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }

    // 检查当前块是否有足够空间
    if (usedSpace + bufferPos_ > BLOCK_SIZE) {
        // 当前块空间不足，先刷写当前块
        memManager_.markDirty(-1, currentLogBlock_);
        memManager_.releasePage(-1, currentLogBlock_);

        // 分配新块
        if (diskManager_.allocBlock(currentLogBlock_) != RC_OK) {
            return RC_OUT_OF_DISK;
        }

        // 获取新块的缓冲帧
        rc = memManager_.getPage(-1, currentLogBlock_, frame, LOG_SPACE);
        if (rc != RC_OK) {
            return rc;
        }
        usedSpace = 0;
    }

    // 将缓存中的日志数据复制到缓冲帧
    memcpy(frame->data + usedSpace, logBuffer_, bufferPos_);
    memManager_.markDirty(-1, currentLogBlock_);
    memManager_.releasePage(-1, currentLogBlock_);

    // 更新日志文件的位置
    logFile_.seekp(currentLogBlock_ * BLOCK_SIZE + usedSpace + bufferPos_);

    // 更新最后刷盘的LSN（缓存中最大的LSN）
    lastFlushedLSN_ = currentLSN_;

    // 重置缓存
    memset(logBuffer_, 0, bufferSize_);
    bufferPos_ = 0;

    return RC_OK;
}

// 刷新日志到磁盘
RC LogManager::flushLog(lsn_t lsn) {
    // 如果请求刷新的LSN小于等于最后刷盘的LSN，无需操作
    if (lsn != MAX_LSN && lsn <= lastFlushedLSN_) {
        return RC_OK;
    }

    // 刷新缓存到内存管理器
    return flushBufferToMemManager();
}

// 写入事务开始日志
lsn_t LogManager::writeBeginLog(TransactionId txId) {
    // 1. 计算日志长度
    int logLen = calculateLogLength(LOG_BEGIN);

    // 2. 检查缓存空间，不足则刷新
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN; // 刷新失败，返回无效LSN
        }
    }

    // 3. 构造日志记录
    TxControlLog* log = reinterpret_cast<TxControlLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_BEGIN;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = 0; // 事务第一条日志，无前驱
    log->header.length = logLen;

    // 4. 更新事务日志链跟踪（记录事务最后一条日志LSN）
    txLastLSN_[txId] = log->header.lsn;

    // 5. 记录LSN与块和偏移量的映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    // 计算当前块已使用空间
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 6. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}

// 写入事务提交日志
lsn_t LogManager::writeCommitLog(TransactionId txId) {
    // 1. 获取事务最后一条日志的LSN（构建日志链）
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        std::cerr << "Commit log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_COMMIT);

    // 3. 检查缓存空间
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    TxControlLog* log = reinterpret_cast<TxControlLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_COMMIT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN; // 链接到事务的上一条日志
    log->header.length = logLen;

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN偏移映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    // 提交日志需要立即刷新（确保事务持久性）
    flushLog(log->header.lsn);

    return log->header.lsn;
}

// 写入事务中止日志
lsn_t LogManager::writeAbortLog(TransactionId txId) {
    // 1. 获取事务最后一条日志的LSN
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        std::cerr << "Abort log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_ABORT);

    // 3. 检查缓存空间
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    TxControlLog* log = reinterpret_cast<TxControlLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_ABORT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN; // 链接到事务的上一条日志
    log->header.length = logLen;

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN偏移映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    // 中止日志需要立即刷新
    flushLog(log->header.lsn);

    return log->header.lsn;
}

// 写入插入操作日志
lsn_t LogManager::writeInsertLog(TransactionId txId, TableId tableId, const RID& rid,
                                 const char* data, int dataLen) {
    if (!data || dataLen <= 0) {
        return RC_INVALID_LSN;
    }

    // 1. 获取事务最后一条日志的LSN
    lsn_t lastLSN = getLastLSN(txId);

    // 2. 计算日志长度（包含数据）
    int logLen = calculateLogLength(LOG_INSERT, dataLen);

    // 3. 检查缓存空间
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    InsertLog* log = reinterpret_cast<InsertLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_INSERT;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = (lastLSN == RC_INVALID_LSN) ? 0 : lastLSN;
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->dataLen = dataLen;
    memcpy(log->data, data, dataLen); // 复制记录数据

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN偏移映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}

// 写入删除操作日志
lsn_t LogManager::writeDeleteLog(TransactionId txId, TableId tableId, const RID& rid,
                                 const char* data, int dataLen) {
    if (!data || dataLen <= 0) {
        return RC_INVALID_LSN;
    }

    // 1. 获取事务最后一条日志的LSN
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        std::cerr << "Delete log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_DELETE, dataLen);

    // 3. 检查缓存空间
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    DeleteLog* log = reinterpret_cast<DeleteLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_DELETE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN;
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->dataLen = dataLen;
    memcpy(log->data, data, dataLen);

    // 5. 更新事务日志链
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN与块映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}

// 其他日志写入方法（update, createTable等）实现类似，省略...

// 获取指定事务的最后一条日志LSN
lsn_t LogManager::getLastLSN(TransactionId txId) const {
    auto it = txLastLSN_.find(txId);
    if (it != txLastLSN_.end()) {
        return it->second;
    }
    return RC_INVALID_LSN;
}

// 读取指定LSN的日志
RC LogManager::readLog(lsn_t lsn, char* buffer, int& len) {
    if (!buffer) {
        return RC_INVALID_ARG;
    }

    auto it = lsnBlockMap_.find(lsn);
    if (it == lsnBlockMap_.end()) {
        return RC_LOG_NOT_FOUND;
    }

    BlockNum blockNum = it->second.first;
    int offset = it->second.second;

    // 从磁盘读取包含该日志的块
    char blockBuffer[BLOCK_SIZE];
    RC rc = diskManager_.readBlock(blockNum, blockBuffer);
    if (rc != RC_OK) {
        return rc;
    }

    // 解析日志头部获取长度
    LogHeader* header = reinterpret_cast<LogHeader*>(blockBuffer + offset);
    len = header->length;

    // 复制日志数据
    if (len > 0) {
        memcpy(buffer, blockBuffer + offset, len);
    }

    return RC_OK;
}

// 遍历指定事务的完整日志链
RC LogManager::traverseTxLog(TransactionId txId, std::vector<char*>& logChain) {
    lsn_t currentLSN = getLastLSN(txId);
    if (currentLSN == RC_INVALID_LSN) {
        return RC_FILE_NOT_FOUND;
    }

    while (currentLSN != 0) {
        int len = 0;
        char* buffer = new char[BLOCK_SIZE];
        if (readLog(currentLSN, buffer, len) != RC_OK) {
            delete[] buffer;
            return RC_LOG_READ_ERROR;
        }

        logChain.push_back(buffer);

        LogHeader* header = reinterpret_cast<LogHeader*>(buffer);
        currentLSN = header->prevLSN;
    }

    // 反转日志链，使其按时间顺序排列
    std::reverse(logChain.begin(), logChain.end());
    return RC_OK;
}

// 写入更新操作日志
lsn_t LogManager::writeUpdateLog(TransactionId txId, TableId tableId, const RID& rid,
                                 const char* oldData, int oldLen, const char* newData, int newLen) {
    // 1. 获取事务最后一条日志的LSN（构建日志链）
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        std::cerr << "Update log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_UPDATE, oldLen, newLen);

    // 3. 检查缓存空间，不足则刷新
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN; // 刷新失败，返回无效LSN
        }
    }

    // 4. 构造日志记录
    UpdateLog* log = reinterpret_cast<UpdateLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_UPDATE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN;
    log->header.length = logLen;
    log->tableId = tableId;
    log->rid = rid;
    log->oldDataLen = oldLen;
    log->newDataLen = newLen;

    // 复制旧数据和新数据
    char* dataPtr = log->data;
    memcpy(dataPtr, oldData, oldLen);
    memcpy(dataPtr + oldLen, newData, newLen);

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN与块和偏移量的映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}

// 写入创建表日志
lsn_t LogManager::writeCreateTableLog(TransactionId txId, TableId tableId, const char* tableName,
                                      int attrCount, const AttrInfo* attrs) {
    // 1. 获取事务最后一条日志的LSN（构建日志链）
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN && currentLSN_ != 0) { // 允许空事务创建表（首次操作）
        std::cerr << "Create table log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算属性数据长度
    int attrsLen = attrCount * sizeof(AttrInfo);
    int logLen = calculateLogLength(LOG_CREATE_TABLE, 0, attrsLen);

    // 3. 检查缓存空间，不足则刷新
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    CreateTableLog* log = reinterpret_cast<CreateTableLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_CREATE_TABLE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = (lastLSN != RC_INVALID_LSN) ? lastLSN : 0;
    log->header.length = logLen;
    log->tableId = tableId;
    log->attrCount = attrCount;
    strncpy(log->tableName, tableName, sizeof(log->tableName) - 1);
    log->tableName[sizeof(log->tableName) - 1] = '\0'; // 确保字符串终止

    // 复制属性信息
    memcpy(log->attrs, attrs, attrsLen);

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN与块和偏移量的映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}

// 写入删除表日志
lsn_t LogManager::writeDropTableLog(TransactionId txId, TableId tableId, const char* tableName) {
    // 1. 获取事务最后一条日志的LSN（构建日志链）
    lsn_t lastLSN = getLastLSN(txId);
    if (lastLSN == RC_INVALID_LSN) {
        std::cerr << "Drop table log error: Transaction " << txId << " not found" << std::endl;
        return RC_INVALID_LSN;
    }

    // 2. 计算日志长度
    int logLen = calculateLogLength(LOG_DROP_TABLE);

    // 3. 检查缓存空间，不足则刷新
    if (bufferPos_ + logLen > bufferSize_) {
        if (flushBufferToMemManager() != RC_OK) {
            return RC_INVALID_LSN;
        }
    }

    // 4. 构造日志记录
    DropTableLog* log = reinterpret_cast<DropTableLog*>(logBuffer_ + bufferPos_);
    log->header.type = LOG_DROP_TABLE;
    log->header.txId = txId;
    log->header.lsn = nextLSN();
    log->header.prevLSN = lastLSN;
    log->header.length = logLen;
    log->tableId = tableId;
    strncpy(log->tableName, tableName, sizeof(log->tableName) - 1);
    log->tableName[sizeof(log->tableName) - 1] = '\0'; // 确保字符串终止

    // 5. 更新事务日志链跟踪
    txLastLSN_[txId] = log->header.lsn;

    // 6. 记录LSN与块和偏移量的映射
    long fileOffset = currentLogBlock_ * BLOCK_SIZE;
    int usedSpace = 0;
    if (logFile_.is_open()) {
        logFile_.seekg(fileOffset, std::ios::beg);
        LogHeader tempHeader;
        while (usedSpace + sizeof(LogHeader) <= BLOCK_SIZE) {
            logFile_.seekg(fileOffset + usedSpace, std::ios::beg);
            if (!logFile_.read(reinterpret_cast<char*>(&tempHeader), sizeof(LogHeader)) ||
                tempHeader.length <= 0 || usedSpace + tempHeader.length > BLOCK_SIZE) {
                break;
            }
            usedSpace += tempHeader.length;
        }
    }
    lsnBlockMap_[log->header.lsn] = {currentLogBlock_, usedSpace + bufferPos_};

    // 7. 移动缓存指针
    bufferPos_ += logLen;

    return log->header.lsn;
}