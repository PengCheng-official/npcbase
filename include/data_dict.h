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

// 数据字典管理类
class DataDict {
public:
    DataDict() = default;
    ~DataDict() = default;

    /**
     * 设置日志管理器
     */
    void setLogManager(LogManager* logManager) { logManager_ = logManager; }

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

private:
    std::vector<TableInfo> tables_;  // 存储所有表信息
    TableId nextTableId_ = 1;        // 下一个可用的表ID
    LogManager* logManager_;         // 日志管理器指针，防止循环依赖
};

#endif  // DATA_DICT_H
