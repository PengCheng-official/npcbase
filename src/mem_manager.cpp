#include "../include/mem_manager.h"
#include <cstdlib>
#include <iostream>

MemManager::MemManager(size_t totalMemSize, DiskManager &diskManager) :
        totalMemSize_(totalMemSize), diskManager_(diskManager), clockHand_(0) {
    // 计算各分区大小
    planCacheSize_ = totalMemSize * PLAN_CACHE_PCT / 100;
    dictCacheSize_ = totalMemSize * DICT_CACHE_PCT / 100;
    dataCacheSize_ = totalMemSize * BUFFER_POOL_PCT / 100;
    logCacheSize_ = totalMemSize * LOG_CACHE_PCT / 100;

    // 计算各分区帧数（向上取整）
    planFrames_ = (planCacheSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dictFrames_ = (dictCacheSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dataFrames_ = (dataCacheSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
    logFrames_ = (logCacheSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;

    totalFrames_ = planFrames_ + dictFrames_ + dataFrames_ + logFrames_;
}

MemManager::~MemManager() {
    // 释放所有缓冲帧数据
    for (auto &frame: frames_) {
        if (frame.data != nullptr) {
            delete[] frame.data;
            frame.data = nullptr;
        }
    }
}

RC MemManager::init() {
    // 初始化缓冲帧
    frames_.resize(totalFrames_);
    for (int i = 0; i < totalFrames_; i++) {
        frames_[i].data = new char[BLOCK_SIZE];
        memset(frames_[i].data, 0, BLOCK_SIZE);

        // 设置内存分区类型
        if (i < planFrames_) {
            frames_[i].spaceType = PLAN_SPACE;
        } else if (i < planFrames_ + dictFrames_) {
            frames_[i].spaceType = DICT_SPACE;
        } else if (i < planFrames_ + dictFrames_ + dataFrames_) {
            frames_[i].spaceType = DATA_SPACE;
        } else {
            frames_[i].spaceType = LOG_SPACE;
        }
    }
    return RC_OK;
}

//RC MemManager::getPage(TableId tableId, PageNum pageNum, BufferFrame*& frame, MemSpaceType spaceType) {
//    int idx = findFrame(tableId, pageNum);
//    if (idx != -1) {
//        // 页面已在缓冲池，更新引用位
//        frames_[idx].refBit = true;
//        frames_[idx].pinCount++;
//        frame = &frames_[idx];
//        return RC_OK;
//    }
//
//    // 页面不在缓冲池，需要获取空闲帧
//    BufferFrame* freeFrame = nullptr;
//    RC rc = getFreeFrame(freeFrame, spaceType);
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    // 初始化新帧
//    freeFrame->tableId = tableId;
//    freeFrame->pageNum = pageNum;
//    freeFrame->isDirty = false;
//    freeFrame->refBit = true;
//    freeFrame->pinCount = 1;
//    freeFrame->spaceType = spaceType;
//
//    // 实际实现中应该从磁盘读取数据
//    memset(freeFrame->data, 0, BLOCK_SIZE);
//
//    frame = freeFrame;
//    return RC_OK;
//}

RC MemManager::getPage(TableId tableId, PageNum pageNum, BufferFrame *&frame, MemSpaceType spaceType) {
    // 1. 检查缓冲池中是否已有该页面
    for (int i = 0; i < totalFrames_; i++) {
        if (frames_[i].isValid && frames_[i].tableId == tableId && frames_[i].pageNum == pageNum) {
            frames_[i].pinCount++;
            frames_[i].refBit = true;  // 修复：用refBit替代isReferenced
            frame = &frames_[i];
            return RC_OK;
        }
    }

    // 2. 缓存未命中，查找空闲帧或置换
    int frameIdx = findFreeFrame(spaceType);  // 使用实现的findFreeFrame
    if (frameIdx == -1) {
        frameIdx = clockReplace(spaceType);  // 执行CLOCK置换
        if (frameIdx == -1) {
            return RC_BUFFER_FULL;
        }
    }

    // 3. 若置换的帧是脏页，先刷盘
    BufferFrame &targetFrame = frames_[frameIdx];
    if (targetFrame.isDirty) {
        RC rc = diskManager_.writeBlock(targetFrame.pageNum, targetFrame.data);  // 使用diskManager_
        if (rc != RC_OK) {
            return rc;
        }
        targetFrame.isDirty = false;
    }

    // 4. 从磁盘读取页面数据
    RC rc = diskManager_.readBlock(pageNum, targetFrame.data);  // 使用diskManager_
    if (rc != RC_OK) {
        if (rc == RC_BLOCK_NOT_FOUND) {
            memset(targetFrame.data, 0, BLOCK_SIZE);  // 新页初始化
        } else {
            return rc;
        }
    }

    // 5. 更新缓冲帧信息
    targetFrame.tableId = tableId;
    targetFrame.pageNum = pageNum;
    targetFrame.spaceType = spaceType;
    targetFrame.pinCount = 1;
    targetFrame.refBit = true;  // 修复：用refBit替代isReferenced
    targetFrame.isValid = true;

    frame = &targetFrame;
    return RC_OK;
}

RC MemManager::releasePage(TableId tableId, PageNum pageNum) {
    int idx = findFrame(tableId, pageNum);
    if (idx == -1) {
        return RC_PAGE_NOT_FOUND;
    }

    if (frames_[idx].pinCount > 0) {
        frames_[idx].pinCount--;
    }
    return RC_OK;
}

RC MemManager::markDirty(TableId tableId, PageNum pageNum) {
    int idx = findFrame(tableId, pageNum);
    if (idx == -1) {
        return RC_PAGE_NOT_FOUND;
    }

    frames_[idx].isDirty = true;
    return RC_OK;
}

RC MemManager::flushPage(TableId tableId, PageNum pageNum) {
    int idx = findFrame(tableId, pageNum);
    if (idx == -1) {
        return RC_PAGE_NOT_FOUND;
    }

    // 实际实现中应该写入磁盘
    frames_[idx].isDirty = false;
    return RC_OK;
}

RC MemManager::flushAllPages() {
    for (auto &frame: frames_) {
        if (frame.isDirty) {
            // 实际实现中应该写入磁盘
            frame.isDirty = false;
        }
    }
    return RC_OK;
}

RC MemManager::getFreeFrame(BufferFrame *&frame, MemSpaceType spaceType) {
    // 先查找未使用的帧
    for (int i = 0; i < totalFrames_; i++) {
        if (frames_[i].pageNum == -1 && frames_[i].spaceType == spaceType && frames_[i].pinCount == 0) {
            frame = &frames_[i];
            return RC_OK;
        }
    }

    // 没有未使用的帧，需要置换
    int replaceIdx = clockReplace(spaceType);
    if (replaceIdx == -1) {
        return RC_BUFFER_FULL;
    }

    // 刷新脏页
    if (frames_[replaceIdx].isDirty) {
        flushPage(frames_[replaceIdx].tableId, frames_[replaceIdx].pageNum);
    }

    frame = &frames_[replaceIdx];
    return RC_OK;
}

int MemManager::clockReplace(MemSpaceType spaceType) {
    int start = clockHand_;

    while (true) {
        // 检查当前帧是否符合置换条件
        if (frames_[clockHand_].spaceType == spaceType && frames_[clockHand_].pinCount == 0) {
            if (!frames_[clockHand_].refBit) {
                // 找到可置换的帧
                int idx = clockHand_;
                clockHand_ = (clockHand_ + 1) % totalFrames_;
                return idx;
            } else {
                // 清除引用位，继续查找
                frames_[clockHand_].refBit = false;
            }
        }

        // 移动指针
        clockHand_ = (clockHand_ + 1) % totalFrames_;

        // 如果回到起点，说明没有可置换的帧
        if (clockHand_ == start) {
            return -1;
        }
    }
}

int MemManager::findFrame(TableId tableId, PageNum pageNum) {
    for (int i = 0; i < totalFrames_; i++) {
        if (frames_[i].tableId == tableId && frames_[i].pageNum == pageNum) {
            return i;
        }
    }
    return -1;
}

int MemManager::findFreeFrame(MemSpaceType spaceType) {
    for (int i = 0; i < totalFrames_; ++i) {
        // 空闲帧条件：未使用（pageNum=-1）、对应分区、未被固定
        if (frames_[i].pageNum == -1 &&
            frames_[i].spaceType == spaceType &&
            frames_[i].pinCount == 0) {
            return i;
        }
    }
    return -1;  // 未找到空闲帧
}
