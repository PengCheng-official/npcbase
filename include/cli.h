#ifndef CLI_H
#define CLI_H

#include "table_manager.h"
#include "test.h"
#include "index_manager.h"
#include <string>
#include <vector>

// 命令行交互类
class CLI {
public:
    /**
     * 构造函数
     * @param tableManager 表管理器引用
     */
    CLI(TableManager &tableManager, Test &test, IndexManager &indexManager);
    ~CLI() = default;

    /**
     * 启动命令行交互
     */
    void run();

private:
    TableManager& tableManager_;  // 表管理器引用
    Test& test_;                  // 测试类引用
    IndexManager& indexManager_;  // 索引管理器引用

    /**
     * 解析命令
     * @param command 命令字符串
     * @param args 输出参数，返回命令参数
     * @return 命令类型
     */
    std::string parseCommand(const std::string& command, std::vector<std::string>& args);

    /**
     * 执行命令
     * @param cmd 命令类型
     * @param args 命令参数
     */
    void executeCommand(const std::string& cmd, const std::vector<std::string>& args);

    /**
     * 打印帮助信息
     */
    void printHelp();

    /**
     * 处理测试
     * @param args 命令参数
     */
    void handleTest(const std::vector<std::string>& args);

    /**
     * 处理创建表命令
     * @param args 命令参数
     */
    void handleCreateTable(const std::vector<std::string>& args);

    /**
     * 处理插入记录命令
     * @param args 命令参数
     */
    void handleInsert(const std::vector<std::string>& args);

    /**
     * 处理删除记录命令
     * @param args 命令参数
     */
    void handleDelete(const std::vector<std::string>& args);

    /**
     * 处理更新记录命令
     * @param args 命令参数
     */
    void handleUpdate(const std::vector<std::string>& args);

    /**
     * 处理查询记录命令
     * @param args 命令参数
     */
    void handleSelect(const std::vector<std::string>& args);

    /**
     * 处理垃圾回收命令
     * @param args 命令参数
     */
    void handleVacuum(const std::vector<std::string>& args);

    /**
     * 创建索引
     * @param args 命令参数
     */
    void handleCreateIndex(const std::vector<std::string>& args);

    /**
     * 展示索引
     * @param args 命令参数
     */
    void handleShowIndex(const std::vector<std::string>& args);
};

#endif  // CLI_H
