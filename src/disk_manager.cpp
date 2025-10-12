#include "../include/disk_manager.h"
#include <cstring>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

DiskManager::DiskManager(size_t diskSize, const std::string &dbName)
        : diskSize_(diskSize), dbName_(dbName) {
    totalBlocks_ = diskSize_ / BLOCK_SIZE;
    if (diskSize_ % BLOCK_SIZE != 0) {
        totalBlocks_++;
    }
}

DiskManager::~DiskManager() {
    // 关闭所有打开的表文件
    for (auto &[tableId, fs]: tableFiles_) {
        if (fs.is_open()) {
            fs.close();
        }
    }
}

RC DiskManager::init() {
    // 创建数据库目录（若不存在）
    try {
        RC rc = createTableFile(DICT_TABLE_ID);
        if (rc != RC_OK && rc != RC_FILE_EXISTS) {
            return rc;
        }
        rc = createLogFile();
        if (rc != RC_OK && rc != RC_FILE_EXISTS) {
            return rc;
        }
    } catch (const std::exception &e) {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC DiskManager::createTableFile(TableId tableId) {
    std::string filePath = getFilePath(tableId);
    // 检查文件是否已存在
    if (fs::exists(filePath)) {
        return RC_FILE_EXISTS;
    }

    // 创建并初始化文件
    std::fstream fs(filePath, std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
        return RC_FILE_ERROR;
    }

    // 写入文件头（初始1个块，未使用）
    TableFileHeader header = {1, 0};
    fs.write(reinterpret_cast<const char *>(&header), sizeof(header));

    // 初始化第一个块（空块）
    char emptyBlock[BLOCK_SIZE] = {0};
    fs.write(emptyBlock, BLOCK_SIZE);

    fs.close();
    return RC_OK;
}

RC DiskManager::openTableFile(TableId tableId) {
    // 若已打开则直接返回
    if (tableFiles_.count(tableId)) {
        return RC_OK;
    }

    std::string filePath = getFilePath(tableId);
    std::fstream fs(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
        return RC_FILE_NOT_FOUND;
    }

    tableFiles_[tableId] = std::move(fs);
    return RC_OK;
}

RC DiskManager::closeTableFile(TableId tableId) {
    auto it = tableFiles_.find(tableId);
    if (it == tableFiles_.end()) {
        return RC_FILE_ERROR;
    }
    if (it->second.is_open()) {
        it->second.close();
    }
    tableFiles_.erase(it);
    return RC_OK;
}

RC DiskManager::readTableFileHeader(TableId tableId, TableFileHeader &header) {
    auto it = tableFiles_.find(tableId);
    if (it == tableFiles_.end() || !it->second.is_open()) {
        return RC_FILE_ERROR;
    }

    // 定位到文件头（文件起始位置）
    it->second.seekg(0);
    it->second.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (it->second.fail()) {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC DiskManager::writeTableFileHeader(TableId tableId, const TableFileHeader &header) {
    auto it = tableFiles_.find(tableId);
    if (it == tableFiles_.end() || !it->second.is_open()) {
        return RC_FILE_ERROR;
    }

    // 定位到文件头
    it->second.seekp(0);
    it->second.write(reinterpret_cast<const char *>(&header), sizeof(header));
    if (it->second.fail()) {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC DiskManager::allocBlock(TableId tableId, BlockNum &blockNum) {
    RC rc = openTableFile(tableId);
    if (rc != RC_OK) {
        return rc;
    }

    TableFileHeader header;
    rc = readTableFileHeader(tableId, header);
    if (rc != RC_OK) {
        std::cerr << "readTableFileHeader " << rc << std::endl;
        return rc;
    }

    // 分配新块（使用第一个未使用的块号）
    blockNum = header.usedBlocks;
    header.usedBlocks++;

    // 若需要扩展文件（已分配块数达到总块数）
    if (header.usedBlocks >= header.totalBlocks) {
        // 扩展1个块（实际可按批次扩展）
        header.totalBlocks++;
        std::string filePath = getFilePath(tableId);
        std::fstream &fs = tableFiles_[tableId];
        // 定位到文件末尾并写入空块
        fs.seekp(0, std::ios::end);
        char emptyBlock[BLOCK_SIZE] = {0};
        fs.write(emptyBlock, BLOCK_SIZE);
        if (fs.fail()) {
            return RC_FILE_NOT_FOUND;
        }
    }

    // 更新文件头
    return writeTableFileHeader(tableId, header);
}

RC DiskManager::freeBlock(TableId tableId, BlockNum blockNum) {
    // 简化实现：仅标记（实际可维护空闲块列表）
    TableFileHeader header;
    RC rc = readTableFileHeader(tableId, header);
    if (rc != RC_OK) {
        return rc;
    }

    if (blockNum < 0 || blockNum >= header.usedBlocks) {
        return RC_INVALID_BLOCK;
    }

    // 实际实现中应将块号加入空闲列表
    return RC_OK;
}

RC DiskManager::readBlock(TableId tableId, BlockNum blockNum, char *data) {
    if (data == nullptr) {
        return RC_INVALID_ARG;
    }

    RC rc = openTableFile(tableId);
    if (rc != RC_OK) {
        return rc;
    }

    TableFileHeader header;
    rc = readTableFileHeader(tableId, header);
    if (rc != RC_OK) {
        return rc;
    }
    if (blockNum >= header.usedBlocks) {
        return RC_BLOCK_NOT_FOUND;
    }

    // 块在文件中的偏移量 = 文件头大小 + 块号 * 块大小
    size_t offset = sizeof(TableFileHeader) + blockNum * BLOCK_SIZE;
    std::fstream &fs = tableFiles_[tableId];
    fs.seekg(offset);
    fs.read(data, BLOCK_SIZE);

    if (fs.fail()) {
        std::cerr << "fs fail!!!! "  << blockNum << std::endl;
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC DiskManager::writeBlock(TableId tableId, BlockNum blockNum, const char *data) {
    if (data == nullptr) {
        return RC_INVALID_ARG;
    }

    RC rc = openTableFile(tableId);
    if (rc != RC_OK) {
        return rc;
    }

    TableFileHeader header;
    rc = readTableFileHeader(tableId, header);
    if (rc != RC_OK) {
        return rc;
    }

    if (blockNum < 0 || blockNum >= header.usedBlocks) {
        return RC_INVALID_BLOCK;
    }

    size_t offset = sizeof(TableFileHeader) + blockNum * BLOCK_SIZE;
    std::fstream &fs = tableFiles_[tableId];
    fs.seekp(offset);
    fs.write(data, BLOCK_SIZE);

    if (fs.fail()) {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC DiskManager::createLogFile() {
    std::string filePath = getFilePath(LOG_TABLE_ID);
    // 检查文件是否已存在
    if (fs::exists(filePath)) {
        return RC_FILE_EXISTS;
    }

    // 创建并初始化文件
    std::fstream fs(filePath, std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
        return RC_FILE_ERROR;
    }

    // 写入文件头（初始1个块，未使用）
    TableFileHeader header = {1, 0};
    fs.write(reinterpret_cast<const char *>(&header), sizeof(header));
    if (fs.fail()) {
        return RC_FILE_NOT_FOUND;
    }

    // 初始化第一个块（空块）
    char emptyBlock[BLOCK_SIZE] = {0};
    fs.write(emptyBlock, BLOCK_SIZE);
    if (fs.fail()) {
        return RC_FILE_NOT_FOUND;
    }

    fs.close();
    return RC_OK;
}
//
//RC DiskManager::openLogFile() {
//    return openTableFile(LOG_TABLE_ID);
//}
//
//RC DiskManager::closeLogFile() {
//    return closeTableFile(LOG_TABLE_ID);
//}
//
//RC DiskManager::allocLogBlock(BlockNum &blockNum) {
//    RC rc = openLogFile();
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    TableFileHeader header;
//    rc = readLogFileHeader(header);
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    // 分配新块（使用第一个未使用的块号）
//    blockNum = header.usedBlocks;
//    header.usedBlocks++;
//
//    // 若需要扩展文件（已分配块数达到总块数）
//    if (header.usedBlocks >= header.totalBlocks) {
//        // 扩展1个块（实际可按批次扩展）
//        header.totalBlocks++;
//        std::string filePath = getLogFilePath();
//        std::fstream &fs = tableFiles_[LOG_TABLE_ID];
//        // 定位到文件末尾并写入空块
//        fs.seekp(0, std::ios::end);
//        char emptyBlock[BLOCK_SIZE] = {0};
//        fs.write(emptyBlock, BLOCK_SIZE);
//        if (fs.fail()) {
//            return RC_FILE_NOT_FOUND;
//        }
//    }
//
//    // 更新文件头
//    return writeLogFileHeader(header);
//}
//
//RC DiskManager::freeLogBlock(BlockNum blockNum) {
//    // 简化实现：仅标记（实际可维护空闲块列表）
//    TableFileHeader header;
//    RC rc = readLogFileHeader(header);
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    if (blockNum < 0 || blockNum >= header.usedBlocks) {
//        return RC_INVALID_BLOCK;
//    }
//
//    // 实际实现中应将块号加入空闲列表
//    return RC_OK;
//}
//
//RC DiskManager::readLogBlock(BlockNum blockNum, char *data) {
//    if (data == nullptr) {
//        return RC_INVALID_ARG;
//    }
//
//    RC rc = openLogFile();
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    TableFileHeader header;
//    rc = readLogFileHeader(header);
//    if (rc != RC_OK) {
//        return rc;
//    }
//    size_t offset = sizeof(TableFileHeader) + blockNum * BLOCK_SIZE;
//
//    // 块在文件中的偏移量 = 文件头大小 + 块号 * 块大小
//    std::fstream &fs = tableFiles_[LOG_TABLE_ID];
//    fs.seekg(offset);
//    fs.read(data, BLOCK_SIZE);
//
//    if (fs.fail()) {
//        return RC_FILE_NOT_FOUND;
//    }
//    return RC_OK;
//}
//
//RC DiskManager::writeLogBlock(BlockNum blockNum, const char *data) {
//    if (data == nullptr) {
//        return RC_INVALID_ARG;
//    }
//
//    RC rc = openLogFile();
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    TableFileHeader header;
//    rc = readLogFileHeader(header);
//    if (rc != RC_OK) {
//        return rc;
//    }
//
//    if (blockNum < 0 || blockNum >= header.usedBlocks) {
//        return RC_INVALID_BLOCK;
//    }
//
//    size_t offset = sizeof(TableFileHeader) + blockNum * BLOCK_SIZE;
//    std::fstream &fs = tableFiles_[LOG_TABLE_ID];
//    fs.seekp(offset);
//    fs.write(data, BLOCK_SIZE);
//
//    if (fs.fail()) {
//        return RC_FILE_NOT_FOUND;
//    }
//    return RC_OK;
//}
//
//RC DiskManager::readLogFileHeader(TableFileHeader &header) {
//    return readTableFileHeader(LOG_TABLE_ID, header);
//}
//
//RC DiskManager::writeLogFileHeader(const TableFileHeader &header) {
//    return writeTableFileHeader(LOG_TABLE_ID, header);
//}

