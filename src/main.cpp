#include "../include/npcbase.h"
#include "../include/data_dict.h"
#include "../include/mem_manager.h"
#include "../include/disk_manager.h"
#include "../include/table_manager.h"
#include "../include/log_manager.h"
#include "../include/index_manager.h"
#include "../include/cli.h"
#include <iostream>
#include <limits>

// 默认配置
//const size_t DEFAULT_MEM_SIZE = 2 * 1024 * 1024;  // 2MB内存
//const size_t DEFAULT_DISK_SIZE = 10 * 1024 * 1024;  // 10MB磁盘
const std::string DEFAULT_DB_NAME = "npcbaseDb";

size_t inputAndAdjustSpaceSize(const std::string& type) {
    size_t inputSize;
    while (true) {
        std::cout << "Please enter the size of the " << type << " (MB): ";
        if (std::cin >> inputSize) {
            inputSize *= 1024 * 1024;
            size_t blocks = inputSize / BLOCK_SIZE;
            if (inputSize % BLOCK_SIZE != 0) {
                blocks += 1;
            }
            return blocks * BLOCK_SIZE;
        } else {
            // 处理非数字输入（避免程序崩溃）
            std::cerr << "Input error! Please enter valid numbers." << std::endl;
            std::cin.clear();  // 清除输入错误状态
            // 忽略缓冲区中剩余的无效输入
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "NPCBase Database System" << std::endl;
    
    // 解析命令行参数（简化实现）
    size_t memSize = inputAndAdjustSpaceSize("Main Memory");
    size_t diskSize = inputAndAdjustSpaceSize("Disk Memory");
    std::string dbName = DEFAULT_DB_NAME;

    // 初始化底层组件
    DiskManager diskManager(diskSize, dbName);
    MemManager memManager(memSize, diskManager);
    LogManager logManager(diskManager, memManager);
    DataDict dataDict(diskManager, memManager, logManager);

    // 初始化数据库
    RC rc = diskManager.init();
    if (rc != RC_OK) {
        std::cerr << "Failed to initialize disk manager: " << rc << std::endl;
        return 1;
    }
    
    rc = memManager.init();
    if (rc != RC_OK) {
        std::cerr << "Failed to initialize memory manager: " << rc << std::endl;
        return 1;
    }

    rc = logManager.init();
    if (rc != RC_OK) {
        std::cerr << "Failed to initialize log manager: " << rc << std::endl;
        return 1;
    }

    rc = dataDict.init();
    if (rc != RC_OK) {
        std::cerr << "Failed to initialize data dictionary: " << rc << std::endl;
        return 1;
    }

    // 构建索引管理器并与表管理器集成
    IndexManager indexManager(dataDict, diskManager, memManager, logManager);
    // 初始化表管理器（集成索引和事务支持）
    TableManager tableManager(dataDict, diskManager, memManager, logManager, indexManager);

    std::cout << "Database initialized successfully." << std::endl;
    std::cout << "Memory size: " << memSize << " bytes" << std::endl;
    std::cout << "Disk size: " << diskSize << " bytes" << std::endl;
    std::cout << "Block size: " << BLOCK_SIZE << " bytes" << std::endl;

    // 创建测试
    Test test(tableManager, memManager, diskManager, dataDict, indexManager);

    // 启动命令行交互
    CLI cli(tableManager, dataDict, test, indexManager);
    cli.run();
    
    // 关闭数据库
    memManager.flushAllPages();
    std::cout << "Database closed!" << std::endl;
    
    return 0;
}
