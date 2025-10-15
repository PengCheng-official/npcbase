#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "npcbase.h"
#include "disk_manager.h"
#include <vector>

// 缓冲帧结构体
struct BufferFrame {
    PageNum pageNum;       // 页号
    TableId tableId;       // 表ID
    char *data;            // 页数据
    bool isValid;          // 合法标记
    bool isDirty;          // 脏页标记
    bool refBit;           // 引用位（CLOCK算法）
    MemSpaceType spaceType;// 内存分区类型
    int pinCount;          // 固定计数


    BufferFrame() : pageNum(-1), tableId(-1), data(nullptr), isValid(true),
                    isDirty(false), refBit(false),
                    spaceType(DATA_SPACE), pinCount(0) {}
};

// 内存管理器类
class MemManager {
public:
    /**
     * 构造函数
     * @param totalMemSize 总内存大小（字节）
     */
    MemManager(size_t totalMemSize, DiskManager &diskManager);
    ~MemManager();

    /**
     * 初始化内存管理器
     */
    RC init();

    /**
     * 从缓冲池获取页
     * @param tableId 表ID
     * @param pageNum 页号
     * @param frame 输出参数，返回缓冲帧
     * @param spaceType 内存分区类型
     */
    RC getPage(TableId tableId, PageNum pageNum, BufferFrame *&frame, MemSpaceType spaceType);

    /**
     * 释放页（减少固定计数）
     * @param tableId 表ID
     * @param pageNum 页号
     */
    RC releasePage(TableId tableId, PageNum pageNum);

    /**
     * 标记页为脏页
     * @param tableId 表ID
     * @param pageNum 页号
     */
    RC markDirty(TableId tableId, PageNum pageNum);

    /**
     * 刷新指定页到磁盘
     * @param tableId 表ID
     * @param pageNum 页号
     */
    RC flushPage(TableId tableId, PageNum pageNum);

    /**
     * 刷新所有脏页到磁盘
     */
    RC flushAllPages();

    /**
     * 刷新指定内存分区的所有脏页到磁盘
     * @param spaceType 内存分区类型
     */
    RC flushSpace(MemSpaceType spaceType);

    /**
     * 获取空闲缓冲帧（可能需要置换）
     * @param frame 输出参数，返回空闲缓冲帧
     * @param spaceType 内存分区类型
     */
    RC getFreeFrame(BufferFrame *&frame, PageNum &pageId, MemSpaceType spaceType);

    size_t totalMemSize_;              // 总内存大小
    size_t planCacheSize_;             // 访问计划区大小
    size_t dictCacheSize_;             // 数据字典区大小
    size_t dataCacheSize_;             // 数据处理缓存区大小
    size_t logCacheSize_;              // 日志缓存区大小

    int totalFrames_;                  // 总帧数
    int planFrames_;                   // 访问计划区帧数
    int dictFrames_;                   // 数据字典区帧数
    int dataFrames_;                   // 数据处理缓存区帧数
    int logFrames_;                    // 日志缓存区帧数

    std::vector<BufferFrame> frames_;  // 缓冲帧数组
    int clockHand_;                    // CLOCK算法指针

private:
    DiskManager& diskManager_;

    /**
     * 执行CLOCK置换算法，找到可置换的页
     * @param spaceType 内存分区类型
     * @return 可置换的帧索引，-1表示失败
     */
    int clockReplace(MemSpaceType spaceType);

    /**
     * 查找缓冲帧
     * @param tableId 表ID
     * @param pageNum 页号
     * @return 找到的帧索引，-1表示未找到
     */
    int findFrame(TableId tableId, PageNum pageNum);

    /**
     * 查找空闲缓冲帧
     * @param spaceType 对应分区
     * @return 找到的帧索引，-1表示未找到
     */
    int findFreeFrame(MemSpaceType spaceType);
};

#endif  // MEM_MANAGER_H