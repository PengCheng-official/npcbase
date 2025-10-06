#include "../include/disk_manager.h"
#include <cstring>
#include <iostream>

DiskManager::DiskManager(size_t diskSize, const std::string& dbName) 
    : diskSize_(diskSize), dbName_(dbName), totalBlocks_(0), blockBitmap_(nullptr) {
    // 计算总块数：磁盘大小 / 块大小
    totalBlocks_ = diskSize_ / BLOCK_SIZE;
    if (diskSize_ % BLOCK_SIZE != 0) {
        totalBlocks_++;
    }
}

DiskManager::~DiskManager() {
    if (dbFile_.is_open()) {
        dbFile_.close();
    }
    delete[] blockBitmap_;
}

RC DiskManager::init() {
    // 计算位图大小（每个块用1位表示）
    size_t bitmapSize = (totalBlocks_ + 7) / 8;
    blockBitmap_ = new char[bitmapSize];
    memset(blockBitmap_, 0, bitmapSize);  // 初始化为0（所有块未分配）

    // 打开或创建数据库文件
    std::string filename = dbName_ + ".db";
    dbFile_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!dbFile_.is_open()) {
        // 文件不存在，创建新文件
        dbFile_.open(filename, std::ios::out | std::ios::binary);
        if (!dbFile_.is_open()) {
            return RC_INVALID_OP;
        }
        
        // 初始化文件大小：写入totalBlocks_个空块
        char buffer[BLOCK_SIZE] = {0};
        for (int i = 0; i < totalBlocks_; i++) {
            dbFile_.write(buffer, BLOCK_SIZE);
        }
        dbFile_.close();
        
        // 重新以读写模式打开
        dbFile_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!dbFile_.is_open()) {
            return RC_INVALID_OP;
        }
    }
    
    return RC_OK;
}

RC DiskManager::allocBlock(BlockNum& blockNum) {
    // 查找第一个未分配的块
    for (int i = 0; i < totalBlocks_; i++) {
        if (!isBlockAllocated(i)) {
            // 标记块为已分配
            size_t byteIdx = i / 8;
            int bitIdx = i % 8;
            blockBitmap_[byteIdx] |= (1 << bitIdx);
            
            blockNum = i;
            return RC_OK;
        }
    }
    
    return RC_OUT_OF_DISK;
}

RC DiskManager::freeBlock(BlockNum blockNum) {
    if (blockNum < 0 || blockNum >= totalBlocks_) {
        return RC_INVALID_ARG;
    }
    
    // 标记块为未分配
    size_t byteIdx = blockNum / 8;
    int bitIdx = blockNum % 8;
    blockBitmap_[byteIdx] &= ~(1 << bitIdx);
    
    return RC_OK;
}

RC DiskManager::readBlock(BlockNum blockNum, char* data) {
    if (blockNum < 0 || blockNum >= totalBlocks_ || data == nullptr) {
        return RC_INVALID_ARG;
    }
    
    if (!isBlockAllocated(blockNum)) {
        return RC_BLOCK_NOT_FOUND;
    }
    
    // 计算偏移量并读取数据
    size_t offset = blockToOffset(blockNum);
    dbFile_.seekg(offset);
    dbFile_.read(data, BLOCK_SIZE);
    
    if (dbFile_.fail()) {
        return RC_INVALID_OP;
    }
    
    return RC_OK;
}

RC DiskManager::writeBlock(BlockNum blockNum, const char* data) {
    if (blockNum < 0 || blockNum >= totalBlocks_ || data == nullptr) {
        return RC_INVALID_ARG;
    }
    
    if (!isBlockAllocated(blockNum)) {
        return RC_BLOCK_NOT_FOUND;
    }
    
    // 计算偏移量并写入数据
    size_t offset = blockToOffset(blockNum);
    dbFile_.seekp(offset);
    dbFile_.write(data, BLOCK_SIZE);
    
    if (dbFile_.fail()) {
        return RC_INVALID_OP;
    }
    
    return RC_OK;
}

bool DiskManager::isBlockAllocated(BlockNum blockNum) {
    if (blockNum < 0 || blockNum >= totalBlocks_) {
        return false;
    }
    
    size_t byteIdx = blockNum / 8;
    int bitIdx = blockNum % 8;
    return (blockBitmap_[byteIdx] & (1 << bitIdx)) != 0;
}

size_t DiskManager::blockToOffset(BlockNum blockNum) const {
    return static_cast<size_t>(blockNum) * BLOCK_SIZE;
}
