#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "npcbase.h"
#include "data_dict.h"
#include "mem_manager.h"
#include "disk_manager.h"

// 变长记录页面头
struct VarPageHeader {
    PageNum pageNum;          // 当前页面的页号（唯一标识）
    int freeOffset;           // 空闲空间起始偏移量
    int recordCount;          // 记录总数
    int deletedCount;         // 已删除记录数
    int freeList[16];         // 空闲空间数组（优化插入）
    int freeListCount;        // 空闲空间数量
};

// 记录槽信息
struct RecordSlot {
    int offset;               // 记录在页中的偏移量
    int length;               // 记录长度
    bool isDeleted;           // 删除标记
};

// 表管理器类
class TableManager {
public:
    /**
     * 构造函数
     * @param dataDict 数据字典引用
     * @param memManager 内存管理器引用
     * @param diskManager 磁盘管理器引用
     */
    TableManager(DataDict& dataDict, MemManager& memManager, DiskManager& diskManager);
    ~TableManager() = default;

    /**
     * 创建表
     * @param tableName 表名
     * @param attrCount 属性数量
     * @param attrs 属性信息数组
     */
    RC createTable(const char* tableName, int attrCount, const AttrInfo* attrs);

    /**
     * 删除表
     * @param tableName 表名
     */
    RC dropTable(const char* tableName);

    /**
     * 插入记录
     * @param tableName 表名
     * @param data 记录数据
     * @param length 记录长度
     * @param rid 输出参数，返回记录ID
     */
    RC insertRecord(const char* tableName, const char* data, int length, RID& rid);

    /**
     * 删除记录
     * @param tableName 表名
     * @param rid 记录ID
     */
    RC deleteRecord(const char* tableName, const RID& rid);

    /**
     * 更新记录
     * @param tableName 表名
     * @param rid 记录ID
     * @param newData 新记录数据
     * @param newLength 新记录长度
     */
    RC updateRecord(const char* tableName, const RID& rid, const char* newData, int newLength);

    /**
     * 读取记录
     * @param tableName 表名
     * @param rid 记录ID
     * @param data 输出参数，返回记录数据
     * @param length 输出参数，返回记录长度
     */
    RC readRecord(const char* tableName, const RID& rid, char*& data, int& length);

    /**
     * 执行垃圾回收（Vacuum）
     * @param tableName 表名
     */
    RC vacuum(const char* tableName);

private:
    DataDict& dataDict_;      // 数据字典引用
    MemManager& memManager_;  // 内存管理器引用
    DiskManager& diskManager_;// 磁盘管理器引用

    /**
     * 初始化新页面
     * @param pageData 页面数据指针
     */
    void initNewPage(char* pageData, PageNum pageNum);

    /**
     * 查找适合插入记录的页面
     * @param tableInfo 表信息
     * @param length 记录长度
     * @param pageNum 输出参数，返回页面号
     * @param frame 输出参数，返回缓冲帧
     */
    RC findPageForInsert(const TableInfo& tableInfo, int length, PageNum& pageNum, BufferFrame*& frame);

    /**
     * 在页面中查找空闲槽位
     * @param pageData 页面数据
     * @param length 记录长度
     * @param slotNum 输出参数，返回槽位号
     * @return 成功返回RC_OK，否则返回错误码
     */
    RC findFreeSlot(char* pageData, int length, SlotNum& slotNum);

    /**
     * 计算记录所需空间
     * @param length 记录数据长度
     */
    int calculateRecordSpace(int length);
};

#endif  // TABLE_MANAGER_H
