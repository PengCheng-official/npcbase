#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include "npcbase.h"
#include <string>
#include <fstream>

// 磁盘管理器类
class DiskManager {
public:
    /**
     * 构造函数：初始化磁盘大小和数据库名称
     * @param diskSize 磁盘大小（字节）
     * @param dbName 数据库名称
     */
    DiskManager(size_t diskSize, const std::string& dbName);
    ~DiskManager();

    /**
     * 初始化磁盘（创建/打开文件、初始化位图）
     */
    RC init();

    /**
     * 分配新块
     * @param blockNum 输出参数，返回分配的块号
     */
    RC allocBlock(BlockNum& blockNum);

    /**
     * 释放块
     * @param blockNum 块号
     */
    RC freeBlock(BlockNum blockNum);

    /**
     * 从块读取数据
     * @param blockNum 块号
     * @param data 数据缓冲区
     */
    RC readBlock(BlockNum blockNum, char* data);

    /**
     * 向块写入数据
     * @param blockNum 块号
     * @param data 数据缓冲区
     */
    RC writeBlock(BlockNum blockNum, const char* data);

    /**
     * 获取块数量
     */
    int getBlockCount() const { return totalBlocks_; }

    /**
     * 检查块是否已分配
     * @param blockNum 块号
     */
    bool isBlockAllocated(BlockNum blockNum);

private:
    size_t diskSize_;          // 磁盘大小
    std::string dbName_;       // 数据库名称
    std::fstream dbFile_;      // 数据库文件流（操作磁盘文件）
    int totalBlocks_;          // 总块数
    char* blockBitmap_;        // 块分配位图（1位表示1个块的分配状态：1=已分配，0=未分配）

    /**
     * 计算块在文件中的偏移量
     * @param blockNum 块号
     */
    size_t blockToOffset(BlockNum blockNum) const;
};

#endif  // DISK_MANAGER_H
