#ifndef NPCBASE_H
#define NPCBASE_H

#include <cstdint>
#include <cstring>
#include <string>

// 公共常量定义
#define BLOCK_SIZE 4096          // 块大小为4KB
#define MAX_TABLE_NAME_LEN 32    // 最大表名长度
#define MAX_ATTR_NAME_LEN 32     // 最大属性名长度
#define MAX_ATTRS_PER_TABLE 16   // 每个表最大属性数
#define MAX_RECORD_LEN (BLOCK_SIZE - 64)  // 最大记录长度
#define BUFFER_POOL_PCT 70       // 数据处理缓存占内存比例
#define PLAN_CACHE_PCT 10        // 访问计划占内存比例
#define DICT_CACHE_PCT 10        // 数据字典占内存比例
#define LOG_CACHE_PCT 10         // 日志缓存占内存比例
#define DICT_TABLE_ID 0          // 数据字典表ID
#define LOG_TABLE_ID (-1)        // 数据字典表ID
#define PLAN_TABLE_ID (-2)       // 数据字典表ID

// 返回码定义
typedef int RC;

#define RC_OK 0                  // 操作成功
#define RC_FILE_EXISTS 1         // 文件已存在
#define RC_FILE_NOT_FOUND 2      // 文件不存在
#define RC_FILE_ERROR 3          // 文件错误
#define RC_INVALID_ARG 4         // 无效参数
#define RC_OUT_OF_MEMORY 5       // 内存不足
#define RC_OUT_OF_DISK 6         // 磁盘空间不足
#define RC_BLOCK_NOT_FOUND 7     // 块不存在
#define RC_INVALID_BLOCK 8       // 块非法
#define RC_PAGE_NOT_FOUND 9      // 页不存在
#define RC_SLOT_NOT_FOUND 10     // 槽不存在
#define RC_RECORD_TOO_LONG 11    // 记录过长
#define RC_BUFFER_FULL 12        // 缓冲池满
#define RC_TABLE_EXISTS 13       // 表已存在
#define RC_TABLE_NOT_FOUND 14    // 表不存在
#define RC_ATTR_NOT_FOUND 15     // 属性不存在
#define RC_INVALID_OP 16         // 无效操作
#define RC_IO_ERROR 17           // IO错误
#define RC_LOG_NOT_FOUND 18      // 日志不存在
#define RC_LOG_FILE_ERROR 19     // 日志文件错误
#define RC_LOG_FLUSH_ERROR 20    // 日志刷新错误
#define RC_INVALID_LSN 21        // 无效LSN
#define RC_LOG_NOT_FLUSHED 22    // 日志缓冲中
#define RC_LOG_READ_ERROR 23     // 日志读取错误

// 数据类型枚举
enum AttrType {
    INT,        // 整数类型
    FLOAT,      // 浮点类型
    STRING      // 字符串类型
};

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

// 内存分区类型
enum MemSpaceType {
    PLAN_SPACE,    // 访问计划区
    DICT_SPACE,    // 数据字典区
    DATA_SPACE,    // 数据处理缓存区
    LOG_SPACE      // 日志缓存区
};

// 页号类型
typedef int32_t PageNum;

// 槽号类型
typedef int16_t SlotNum;

// 块号类型
typedef int32_t BlockNum;

// 表ID类型
typedef int32_t TableId;

// 事务ID类型
typedef int32_t TransactionId;

// 日志序列号类型（LSN）
typedef int64_t lsn_t;

// 属性信息结构体
struct AttrInfo {
    char name[MAX_ATTR_NAME_LEN];  // 属性名
    AttrType type;                 // 属性类型
    int length;                    // 属性长度（字符串类型有效）
};

// 记录ID结构体
struct RID {
    PageNum pageNum;  // 页号
    SlotNum slotNum;  // 槽号

    RID() : pageNum(-1), slotNum(-1) {}
    RID(PageNum p, SlotNum s) : pageNum(p), slotNum(s) {}

    bool operator==(const RID& other) const {
        return pageNum == other.pageNum && slotNum == other.slotNum;
    }
};

// 最大LSN值
#define MAX_LSN INT64_MAX


#endif  // NPCBASE_H
