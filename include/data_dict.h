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

// 索引信息结构体（sys_indexes）
struct IndexInfo {
    TableId indexId;                        // 索引文件ID（独立文件）
    char indexName[MAX_TABLE_NAME_LEN];     // 索引名
    TableId tableId;                        // 所属表ID
    char tableName[MAX_TABLE_NAME_LEN];     // 所属表名（便于展示）
    char columnName[MAX_ATTR_NAME_LEN];     // 索引列名（单列）
    AttrType keyType;                       // 键类型
    int keyLen;                             // 键长度
    PageNum rootPage;                       // 根页号
    bool unique;                            // 是否唯一
    int height;                             // 树高
    int totalPages;                         // 页面总数
    int totalKeys;                          // 键总数
};

struct DictPageHeader {
    int tableCount;  // 该页实际存储的表元数据数量
};

// 数据字典管理类
class DataDict {
public:
    DataDict(DiskManager &diskManager, MemManager &memManager, LogManager &logManager);
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

    // ========= 索引元数据（sys_indexes）=========
    /**
     * 创建索引元数据并创建对应文件（不构建数据）
     * @param indexName 索引名
     * @param tableName 表名
     * @param columnName 列名
     * @param unique 是否唯一
     * @param outIndex 输出参数，返回创建的索引信息
     */
    RC createIndexMetadata(TransactionId txId, const char* indexName, const char* tableName,
                           const char* columnName, bool unique, IndexInfo& outIndex);

    /**
     * 查找索引
     * @param indexName 索引名
     * @param outIndex 输出参数，返回索引信息
     */
    RC findIndex(const char* indexName, IndexInfo& outIndex);

    /**
     * 列出表的所有索引
     * @param tableId 表ID
     * @param outIndexes 输出参数，返回索引信息列表
     */
    RC listIndexesForTable(TableId tableId, std::vector<IndexInfo>& outIndexes);

    /**
     * 更新索引信息（根页、树高、统计等）
     * @param info 索引信息
     */
    RC updateIndexInfo(const IndexInfo& info);

private:
    std::vector<TableInfo> tables_;  // 存储所有表信息
    std::vector<IndexInfo> indexes_; // 存储所有索引信息
    TableId nextTableId_ = 1;        // 下一个可用的表ID
    TableId nextIndexId_ = 10000;    // 下一个可用的索引ID（与表ID空间分离）
    DiskManager& diskManager_;       // 磁盘管理器引用
    MemManager& memManager_;         // 内存管理器引用
    LogManager& logManager_;         // 日志管理器引用
    std::unordered_map<BlockNum, int> blockOffsets_;  // 数据字典块偏移
    BlockNum currentLogBlock_;       // 当前数据字典块号（表元数据）

    // sys_indexes 持久化管理
    std::unordered_map<BlockNum, int> indexMetaBlockOffsets_; // 索引元数据块偏移
    BlockNum indexMetaCurrentBlock_ = -1;                     // 当前索引元数据块号

    std::unordered_map<TableId, PageNum> tableIdToDictPage_;  // 表ID到数据字典页面的映射

    // 内部：将表信息写入数据字典缓存
    RC writeToDictCache(const TableInfo &table);

    // 内部：追加写入索引元数据记录
    RC appendIndexMeta(const IndexInfo& info);
};

#endif  // DATA_DICT_H
