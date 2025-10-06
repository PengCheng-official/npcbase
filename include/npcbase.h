//
// Created by 彭诚 on 2025/10/3.
//

#ifndef NPCBASE_NPCBASE_H
#define NPCBASE_NPCBASE_H

#include <vector>
#include <string>

// 常量定义
const int DEFAULT_BLOCK_SIZE = 8 * 1024;  // 默认块大小：8KB
const int SYSTEM_TABLE_INIT_BLOCK_NUM = 10;  // 系统表初始分配磁盘块数
const std::string MEM_BLOCK_PREFIX = "M_";   // 主存块ID前缀
const std::string DISK_BLOCK_PREFIX = "D_";  // 磁盘块ID前缀

// 数据字典-表元数据
struct DictTable {
    int table_id;               // 表唯一ID
    std::string table_name;     // 表名
    int block_count;            // 已分配块数量
    std::string first_block_id; // 第一个块ID（块链头）
    std::vector<std::string> col_names;  // 字段名列表
    std::vector<std::string> col_types;  // 字段类型列表
};

// 数据字典-块元数据
struct DictBlock {
    std::string block_id;       // 块唯一ID（M_xxx/D_xxx）
    std::string block_type;     // 块类型（"SYSTEM"/"TABLE_DATA"/"FREE"）
    int table_id;               // 所属表ID（系统块为0）
    int free_space;             // 空闲空间（字节）
};

// 块结构体（主存/磁盘块通用）
struct Block {
    std::string block_id;       // 块ID
    std::string block_type;     // 块类型
    int table_id;               // 所属表ID
    int free_space;             // 空闲空间（字节）
    std::vector<char> data;     // 块数据（二进制流）

    // 构造函数：初始化块大小
    Block(std::string id, std::string type, int tid)
            : block_id(id), block_type(type), table_id(tid),
              free_space(DEFAULT_BLOCK_SIZE), data(DEFAULT_BLOCK_SIZE, 0) {}
};

// 记录结构体（变长记录）
struct Record {
    std::vector<std::string> values;  // 记录值列表

    // 计算记录总大小（字节）
    int getSize(const std::vector<std::string>& col_types) const;
};

#endif // NPCBASE_NPCBASE_H

