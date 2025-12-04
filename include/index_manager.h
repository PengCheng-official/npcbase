#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "npcbase.h"
#include "data_dict.h"
#include "mem_manager.h"
#include "disk_manager.h"
#include "log_manager.h"
#include <string>
#include <vector>

// 节点类型
enum class IndexNodeType : uint8_t { LEAF = 1, INTERNAL = 2 };

// 索引页头（固定32字节）
struct IndexPageHeader {
    uint8_t nodeType;     // 1字节：叶子/内部
    int32_t pageNum;      // 4字节：页号
    int32_t prevPage;     // 4字节：前驱页（叶子链）
    int32_t nextPage;     // 4字节：后继页（叶子链）
    int16_t keyCount;     // 2字节：当前键数量
    int16_t maxKeys;      // 2字节：最大键数量
    int32_t parentPage;   // 4字节：父节点页号
    int32_t leftMostChild;// 4字节：内部节点用，最左孩子页号；叶子为-1
    uint8_t reserved[7];  // 填充到32字节
};

// 将任意键值抽象为定长字节数组
struct KeyBytes {
    std::vector<char> bytes; // 长度等于keyLen

    KeyBytes() = default;
    explicit KeyBytes(size_t len) : bytes(len, 0) {}

    int compare(const KeyBytes& other) const {
        size_t n = std::min(bytes.size(), other.bytes.size());
        int c = std::memcmp(bytes.data(), other.bytes.data(), n);
        if (c != 0) return c;
        if (bytes.size() == other.bytes.size()) return 0;
        return bytes.size() < other.bytes.size() ? -1 : 1;
    }
};

// 叶子项：key + RID(8字节：4字节页号 + 4字节槽)
struct LeafEntry {
    KeyBytes key;
    int32_t ridPage; // RID.pageNum
    int32_t ridSlot; // 扩展到4字节
};

// 内部项：key + child(8字节，实际使用childPage + 4字节保留)
struct InternalEntry {
    KeyBytes key;
    int32_t childPage; // 指向右侧孩子
    int32_t pad;       // 保留
};

// 索引管理器
class IndexManager {
public:
    IndexManager(DataDict& dataDict, DiskManager& diskManager, MemManager& memManager, LogManager& logManager);
    ~IndexManager() = default;

    // 创建索引：构建B+树并持久化
    RC createIndex(TransactionId txId, const char* indexName, const char* tableName, const char* columnName, bool unique = false);

    // 显示索引文件内容
    RC showIndex(const char* indexName);

    // 由表管理器回调：插入/删除记录时维护索引
    RC onRecordInserted(const TableInfo& table, const char* data, int len, const RID& rid);
    RC onRecordDeleted(const TableInfo& table, const char* data, int len, const RID& rid);

private:
    DataDict& dataDict_;
    DiskManager& diskManager_;
    MemManager& memManager_;
    LogManager& logManager_;

    // 辅助：根据表/列提取键配置
    RC getKeyConfig(const char* tableName, const char* columnName, AttrType& type, int& keyLen);

    // 辅助：将记录数据提取为KeyBytes（当前假设键位于记录起始处）
    KeyBytes extractKey(const char* data, int len, AttrType type, int keyLen);

    // 页面操作
    RC initNewIndexRoot(TableId indexId, PageNum rootPage, int maxKeys, bool leaf);
    RC readPage(TableId indexId, PageNum pageNum, BufferFrame*& frame);
    void releasePage(TableId indexId, PageNum pageNum);

    // B+树操作
    RC insertKey(TableId indexId, const IndexInfo& info, const KeyBytes& key, const RID& rid);
    RC deleteKey(TableId indexId, const IndexInfo& info, const KeyBytes& key, const RID& rid);
    RC splitLeafAndInsert(TableId indexId, const IndexInfo& info, BufferFrame* leafFrame, const KeyBytes& key, const RID& rid);
    RC insertIntoParent(TableId indexId, const IndexInfo& info, PageNum left, const KeyBytes& upKey, PageNum right);
    RC splitInternalAndInsert(TableId indexId, const IndexInfo& info, BufferFrame* internalFrame, const KeyBytes& upKey, PageNum right);

    // 搜索定位叶子
    RC findLeaf(TableId indexId, const IndexInfo& info, const KeyBytes& key, PageNum& leafPage, std::vector<PageNum>* path = nullptr);

    // 计算每页最大项数量
    int calcMaxKeys(int keyLen) const { return (int)((BLOCK_SIZE - sizeof(IndexPageHeader)) / (keyLen + 8)); }

    // ===== 删除重平衡（借位/合并）辅助 =====
    RC rebalanceAfterDelete(TableId indexId, const IndexInfo& info, PageNum leafPage);
    int minKeysForNode(int maxKeys) const { return (maxKeys + 1) / 2; }
    int32_t getChildAt(char* parentPageData, int keyLen, int childIndex) const;
    int findChildIndex(char* parentPageData, int keyLen, PageNum childPage) const;
    RC updateParentKeyForRightChild(TableId indexId, BufferFrame* parentFrame, int keyLen, int keyPos, BufferFrame* rightChildFrame);
    RC removeParentEntryAt(TableId indexId, const IndexInfo& info, BufferFrame* parentFrame, int removeKeyPos);
    RC shrinkRootIfNeeded(TableId indexId, const IndexInfo& info, BufferFrame* rootFrame);

    // 内部节点删除后的重平衡（递归）
    RC rebalanceInternalAfterDelete(TableId indexId, const IndexInfo& info, PageNum parentPage);
    RC setChildrenParent(TableId indexId, const std::vector<PageNum>& children, PageNum newParent);
};

#endif // INDEX_MANAGER_H
