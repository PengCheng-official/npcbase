#ifndef DATA_DICT_H
#define DATA_DICT_H

#include "npcbase.h"
#include "log_manager.h"
#include <vector>
#include <string>

// 表信息结构体
struct TableInfo {
    TableId tableId;                     // 表ID
    char tableName[MAX_TABLE_NAME_LEN];  // 表名
    int attrCount;                       // 属性数量
    AttrInfo attrs[MAX_ATTRS_PER_TABLE]; // 属性信息数组
    PageNum firstPage;                   // 第一个数据页
    PageNum lastPage;                    // 最后一个数据页
    int deletedCount;                    // 被删除的记录数
    int recordCount;                     // 记录总数
};

struct DictPageHeader {
    int tableCount;  // 该页实际存储的表元数据数量
};

// 数据字典管理类
class DataDict {
public:
    DataDict(MemManager &memManager, LogManager &logManager);
    ~DataDict() = default;

    /**
     * 初始化数据字典
     */
    RC init();

    /**
     * 创建新表并添加到数据字典
     * @param tableName 表名
     * @param attrCount 属性数量
     * @param attrs 属性信息数组
     * @param tableId 输出参数，返回表ID
     */
    RC createTable(TransactionId txId, const char *tableName, int attrCount, const AttrInfo *attrs, TableId &tableId);

    /**
     * 删除表
     * @param tableName 表名
     */
    RC dropTable(TransactionId txId, const char *tableName);

    /**
     * 查找表信息
     * @param tableName 表名
     * @param tableInfo 输出参数，返回表信息
     */
    RC findTable(const char* tableName, TableInfo& tableInfo);

    /**
     * 查找表信息（通过表ID）
     * @param tableId 表ID
     * @param tableInfo 输出参数，返回表信息
     */
    RC findTableById(TableId tableId, TableInfo& tableInfo);

    /**
     * 更新表的页面信息
     * @param tableId 表ID
     * @param lastPage 最后一个数据页
     * @param recordCount 记录总数
     */
    RC updateTableInfo(TableId tableId, PageNum lastPage, int recordCount);

    /**
     * 获取所有表名
     * @param tables 输出参数，返回表名列表
     */
    RC listTables(std::vector<std::string>& tables);

    /**
     * 将表信息写入数据字典缓存
     * @param table 表信息
     */
    RC writeToDictCache(const TableInfo &table);

private:
    std::vector<TableInfo> tables_;  // 存储所有表信息
    TableId nextTableId_ = 1;        // 下一个可用的表ID
    MemManager& memManager_;         // 内存管理器引用
    LogManager& logManager_;         // 日志管理器引用

    std::unordered_map<TableId, PageNum> tableIdToDictPage_;  // 表ID到数据字典页面的映射
};

#endif  // DATA_DICT_H
