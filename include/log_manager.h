//
// Created by 彭诚 on 2025/10/5.
//

#ifndef NPCBASE_LOG_MANAGER_H
#define NPCBASE_LOG_MANAGER_H

#include "npcbase.h"
#include "disk_manager.h"
#include "mem_manager.h"
#include <fstream>
#include <unordered_map>
#include <vector>

// 日志类型（参考Redbase设计）
enum LogType {
    LOG_BEGIN,        // 事务开始
    LOG_COMMIT,       // 事务提交
    LOG_ABORT,        // 事务中止
    LOG_INSERT,       // 插入记录
    LOG_DELETE,       // 删除记录
    LOG_UPDATE,       // 更新记录
    LOG_CREATE_TABLE, // 创建表
    LOG_DROP_TABLE,   // 删除表
    LOG_ALTER_TABLE   // 修改表结构
};

// 日志记录头部（所有日志的公共部分）
struct LogHeader {
    LogType type;       // 日志类型
    int length;         // 日志总长度（包括头部）
    TransactionId txId; // 事务ID
    lsn_t lsn;          // 当前日志序列号
    lsn_t prevLSN;      // 同一事务的前一条日志序列号（日志链关键字段）
};

// 事务控制日志（BEGIN/COMMIT/ABORT）
struct TxControlLog {
    LogHeader header;
};

// 插入操作日志
struct InsertLog {
    LogHeader header;
    TableId tableId;    // 表ID
    RID rid;            // 记录ID
    int dataLen;        // 数据长度
    char data[0];       // 柔性数组存储记录数据（长度由dataLen指定）
};

// 删除操作日志
struct DeleteLog {
    LogHeader header;
    TableId tableId;    // 表ID
    RID rid;            // 记录ID
    int dataLen;        // 被删除数据的长度
    char data[0];       // 存储被删除的记录数据（用于回滚）
};

// 更新操作日志
struct UpdateLog {
    LogHeader header;
    TableId tableId;    // 表ID
    RID rid;            // 记录ID
    int oldDataLen;     // 旧数据长度
    int newDataLen;     // 新数据长度
    char data[0];       // 先存旧数据，再存新数据（旧数据长度oldDataLen）
};

// 创建表日志
struct CreateTableLog {
    LogHeader header;
    TableId tableId;        // 表ID
    int attrCount;          // 属性数量
    char tableName[256];    // 表名
    AttrInfo attrs[0];      // 柔性数组存储属性信息（数量由attrCount指定）
};

// 删除表日志
struct DropTableLog {
    LogHeader header;
    TableId tableId;        // 表ID
    char tableName[256];    // 表名
};

// 日志管理器类（完整实现日志链和WAL机制）
class LogManager {
public:
    // 构造函数：依赖磁盘管理器和内存管理器
    LogManager(DiskManager &diskManager, MemManager &memManager);
    ~LogManager();

    // 初始化日志管理器（打开日志文件、初始化缓存）
    RC init();

    // 写入事务开始日志，返回当前日志LSN
    lsn_t writeBeginLog(TransactionId txId);

    // 写入事务提交日志，返回当前日志LSN
    lsn_t writeCommitLog(TransactionId txId);

    // 写入事务中止日志，返回当前日志LSN
    lsn_t writeAbortLog(TransactionId txId);

    // 写入插入操作日志，返回当前日志LSN
    lsn_t writeInsertLog(TransactionId txId, TableId tableId, const RID& rid,
                         const char* data, int dataLen);

    // 写入删除操作日志，返回当前日志LSN
    lsn_t writeDeleteLog(TransactionId txId, TableId tableId, const RID& rid,
                         const char* data, int dataLen);

    // 写入更新操作日志，返回当前日志LSN
    lsn_t writeUpdateLog(TransactionId txId, TableId tableId, const RID& rid,
                         const char* oldData, int oldLen, const char* newData, int newLen);

    // 写入创建表日志，返回当前日志LSN
    lsn_t writeCreateTableLog(TransactionId txId, TableId tableId, const char* tableName,
                              int attrCount, const AttrInfo* attrs);

    // 写入删除表日志，返回当前日志LSN
    lsn_t writeDropTableLog(TransactionId txId, TableId tableId, const char* tableName);

    // 刷新日志到磁盘（默认刷新所有日志）
    RC flushLog(lsn_t lsn = MAX_LSN);

    // 读取指定LSN的日志（用于恢复）
    RC readLog(lsn_t lsn, char* buffer, int& len);

    // 遍历指定事务的完整日志链（用于恢复时回滚/重做）
    RC traverseTxLog(TransactionId txId, std::vector<char*>& logChain);

    // 获取当前最大LSN
    lsn_t getCurrentLSN() const { return currentLSN_; }

    // 获取指定事务的最后一条日志LSN（用于构建日志链）
    lsn_t getLastLSN(TransactionId txId) const;

private:
    DiskManager& diskManager_;       // 磁盘管理器引用
    MemManager& memManager_;         // 内存管理器引用
    std::fstream logFile_;           // 日志文件流
    std::string logFileName_;        // 日志文件名
    lsn_t currentLSN_;               // 当前日志序列号（全局递增）
    lsn_t lastFlushedLSN_;           // 最后刷新到磁盘的LSN
    char* logBuffer_;                // 日志缓存
    int bufferSize_;                 // 日志缓存大小（通常为BLOCK_SIZE）
    int bufferPos_;                  // 缓存当前写入位置
    BlockNum currentLogBlock_;       // 当前日志块号

    // 事务日志链跟踪：记录每个事务的最后一条日志LSN
    std::unordered_map<TransactionId, lsn_t> txLastLSN_;

    // 日志文件索引：LSN到文件偏移量的映射（加速读取）
    std::unordered_map<lsn_t, std::pair<BlockNum, int>> lsnBlockMap_;  // 块号和块内偏移

    // 生成下一个LSN（原子递增）
    lsn_t nextLSN() { return ++currentLSN_; }

    // 将日志缓存写入内存管理器，由其负责刷盘
    RC flushBufferToMemManager();

    // 计算日志记录的总长度（包含头部和数据）
    int calculateLogLength(LogType type, int dataLen = 0, int extraLen = 0);
};
#endif //NPCBASE_LOG_MANAGER_H