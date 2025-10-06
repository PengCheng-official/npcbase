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

// 返回码定义
typedef int RC;

#define RC_OK 0                  // 操作成功
#define RC_FILE_EXISTS 1         // 文件已存在
#define RC_FILE_NOT_FOUND 2      // 文件不存在
#define RC_INVALID_ARG 3         // 无效参数
#define RC_OUT_OF_MEMORY 4       // 内存不足
#define RC_OUT_OF_DISK 5         // 磁盘空间不足
#define RC_BLOCK_NOT_FOUND 6     // 块不存在
#define RC_PAGE_NOT_FOUND 7      // 页不存在
#define RC_SLOT_NOT_FOUND 8      // 槽不存在
#define RC_RECORD_TOO_LONG 9     // 记录过长
#define RC_BUFFER_FULL 10        // 缓冲池满
#define RC_TABLE_EXISTS 11       // 表已存在
#define RC_TABLE_NOT_FOUND 12    // 表不存在
#define RC_ATTR_NOT_FOUND 13     // 属性不存在
#define RC_INVALID_OP 14         // 无效操作

// 数据类型枚举
enum AttrType {
    INT,        // 整数类型
    FLOAT,      // 浮点类型
    STRING      // 字符串类型
};

// 页号类型
typedef int32_t PageNum;

// 槽号类型
typedef int16_t SlotNum;

// 块号类型
typedef int32_t BlockNum;

// 表ID类型
typedef int32_t TableId;

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

// 内存分区类型
enum MemSpaceType {
    PLAN_SPACE,    // 访问计划区
    DICT_SPACE,    // 数据字典区
    DATA_SPACE,    // 数据处理缓存区
    LOG_SPACE      // 日志缓存区
};

#endif  // NPCBASE_H
