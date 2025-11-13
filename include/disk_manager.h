#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include "npcbase.h"
#include <string>
#include <fstream>
#include <unordered_map>


// 表文件头（每个表文件的第一个块）
struct TableFileHeader {
    int totalBlocks;  // 该表文件总块数
    int usedBlocks;   // 已使用块数
};

// 磁盘管理器类（一个表一个文件）
class DiskManager {
public:
    DiskManager(size_t diskSize, const std::string& dbName);
    ~DiskManager();

    /**
     * 初始化数据库目录
     */
    RC init();

    /**
     * 获取数据库名称
     */
    std::string getDbName() const {
        return dbName_;
    }

    /**
     * 为新表创建文件
     * @param tableId 表ID
     */
    RC createTableFile(TableId tableId);

    /**
     * 打开已有表文件
     * @param tableId 表ID
     */
    RC openTableFile(TableId tableId);

    /**
     * 关闭表文件
     * @param tableId 表ID
     */
    RC closeTableFile(TableId tableId);

    /**
     * 为表分配新块
     * @param tableId 表ID
     * @param blockNum 输出参数，返回块号（表内唯一）
     */
    RC allocBlock(TableId tableId, BlockNum& blockNum);

    /**
     * 释放表的块
     * @param tableId 表ID
     * @param blockNum 块号
     */
    RC freeBlock(TableId tableId, BlockNum blockNum);

    /**
     * 从表的块读取数据
     * @param tableId 表ID
     * @param blockNum 块号
     * @param data 数据缓冲区
     */
    RC readBlock(TableId tableId, BlockNum blockNum, char* data);

    /**
     * 向表的块写入数据
     * @param tableId 表ID
     * @param blockNum 块号
     * @param data 数据缓冲区
     */
    RC writeBlock(TableId tableId, BlockNum blockNum, const char* data);

    /**
     * 为新日志创建文件
     */
    RC createLogFile();

    /**
     * 获取表文件路径
     * @param tableId 表ID
     */
    std::string getFilePath(TableId tableId) const {
        if (tableId == LOG_TABLE_ID) {
            return dbName_ + ".log";
        } else if (tableId == PLAN_TABLE_ID) {
            return "";
        } else if (tableId == INDEX_META_TABLE_ID) {
            return dbName_ + "_indexes.db"; // 独立的索引元数据文件
        } else {
            return dbName_ + std::to_string(tableId) + ".db";
        }
    }

    /**
     * 读取表文件头
     * @param tableId 表ID
     * @param header 输出参数，文件头
     */
    RC readTableFileHeader(TableId tableId, TableFileHeader& header);

    /**
     * 写入表文件头
     * @param tableId 表ID
     * @param header 文件头
     */
    RC writeTableFileHeader(TableId tableId, const TableFileHeader& header);

private:
    size_t diskSize_;          // 磁盘大小
    std::string dbName_;       // 数据库名称
    int totalBlocks_;          // 总块数
    // 表ID到文件流的映射（仅打开的文件）
    std::unordered_map<TableId, std::fstream> tableFiles_;
};

#endif  // DISK_MANAGER_H
