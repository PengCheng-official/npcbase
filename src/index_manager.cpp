#include "../include/index_manager.h"
#include "../include/table_manager.h" // for page structures
#include <iostream>
#include <cstring>
#include <algorithm>

IndexManager::IndexManager(DataDict &dataDict, DiskManager &diskManager, MemManager &memManager, LogManager &logManager)
        : dataDict_(dataDict), diskManager_(diskManager), memManager_(memManager), logManager_(logManager) {}

RC IndexManager::getKeyConfig(const char *tableName, const char *columnName, AttrType &type, int &keyLen) {
    TableInfo t;
    RC rc = dataDict_.findTable(tableName, t);
    if (rc != RC_OK) return rc;
    for (int i = 0; i < t.attrCount; ++i) {
        if (strcmp(t.attrs[i].name, columnName) == 0) {
            type = t.attrs[i].type;
            keyLen = (type == STRING) ? t.attrs[i].length : 4;
            return RC_OK;
        }
    }
    return RC_ATTR_NOT_FOUND;
}

KeyBytes IndexManager::extractKey(const char *data, int len, AttrType type, int keyLen) {
    KeyBytes kb(keyLen);
    // 简化：默认索引列在起始偏移（单列表或列为第一个）
    // 更完整实现应根据DataDict计算列偏移
    int copyLen = std::min(keyLen, len);
    std::memcpy(kb.bytes.data(), data, copyLen);
    if (type == STRING && copyLen < keyLen) {
        // zero-pad
        std::memset(kb.bytes.data() + copyLen, 0, keyLen - copyLen);
    }
    return kb;
}

// ===== New: createIndex implementation =====
RC IndexManager::createIndex(TransactionId txId, const char* indexName, const char* tableName, const char* columnName, bool unique) {
    if (!indexName || !tableName || !columnName) return RC_INVALID_ARG;

    // 1) 在数据字典中创建索引元数据（并创建索引文件）
    IndexInfo info{};
    RC rc = dataDict_.createIndexMetadata(txId, indexName, tableName, columnName, unique, info);
    if (rc != RC_OK) return rc;

    // 2) 分配并初始化根页（叶子）
    int maxKeys = calcMaxKeys(info.keyLen);
    BlockNum rootBlock;
    rc = diskManager_.allocBlock(info.indexId, rootBlock);
    if (rc != RC_OK) return rc;
    rc = initNewIndexRoot(info.indexId, rootBlock, maxKeys, true);
    if (rc != RC_OK) return rc;

    info.rootPage = rootBlock;
    info.height = 1;

    // 3) 将当前统计写回（pages等由文件头决定）
    TableFileHeader fh; rc = diskManager_.readTableFileHeader(info.indexId, fh);
    if (rc == RC_OK) {
        info.totalPages = fh.usedBlocks;
    }
    info.totalKeys = 0;

    dataDict_.updateIndexInfo(info);

    // 4) 从现有表数据回填构建索引
    TableInfo table;
    rc = dataDict_.findTable(tableName, table);
    if (rc != RC_OK) return rc;

    if (table.firstPage != -1 && table.recordCount > 0) {
        // 顺序扫描（简化：假设页连续）
        for (PageNum p = table.firstPage; p <= table.lastPage; ++p) {
            BufferFrame* f = nullptr; RC rr = memManager_.getPage(table.tableId, p, f, DATA_SPACE);
            if (rr != RC_OK) continue;
            auto* vh = reinterpret_cast<VarPageHeader*>(f->data);
            int slots = vh->recordCount + vh->deletedCount;
            for (int s = 0; s < slots; ++s) {
                auto* slot = reinterpret_cast<RecordSlot*>(f->data + sizeof(VarPageHeader) + s * sizeof(RecordSlot));
                if (slot->isDeleted) continue;
                char* rec = f->data + slot->offset; int len = slot->length;
                KeyBytes kb = extractKey(rec, len, info.keyType, info.keyLen);
                RC irc = insertKey(info.indexId, info, kb, RID(p, (SlotNum)s));
                if (irc == RC_OK) info.totalKeys++;
            }
            memManager_.releasePage(table.tableId, p);
        }
        // 更新统计（usedBlocks可能增长）
        if (diskManager_.readTableFileHeader(info.indexId, fh) == RC_OK) {
            info.totalPages = fh.usedBlocks;
        }
        dataDict_.updateIndexInfo(info);
    }

    return RC_OK;
}

RC IndexManager::initNewIndexRoot(TableId indexId, PageNum rootPage, int maxKeys, bool leaf) {
    BufferFrame* frame = nullptr;
    RC rc = memManager_.getPage(indexId, rootPage, frame, DATA_SPACE);
    if (rc != RC_OK) return rc;
    std::memset(frame->data, 0, BLOCK_SIZE);
    auto* hdr = reinterpret_cast<IndexPageHeader*>(frame->data);
    hdr->nodeType = leaf ? (uint8_t)IndexNodeType::LEAF : (uint8_t)IndexNodeType::INTERNAL;
    hdr->pageNum = rootPage;
    hdr->prevPage = -1;
    hdr->nextPage = -1;
    hdr->keyCount = 0;
    hdr->maxKeys = (int16_t)maxKeys;
    hdr->parentPage = -1;
    hdr->leftMostChild = leaf ? -1 : -1;
    memManager_.markDirty(indexId, rootPage);
    memManager_.releasePage(indexId, rootPage);
    return RC_OK;
}

RC IndexManager::readPage(TableId indexId, PageNum pageNum, BufferFrame *&frame) {
    return memManager_.getPage(indexId, pageNum, frame, DATA_SPACE);
}

void IndexManager::releasePage(TableId indexId, PageNum pageNum) {
    memManager_.releasePage(indexId, pageNum);
}

static inline char* leafEntryPtr(char* base, int keyLen, int pos) {
    return base + sizeof(IndexPageHeader) + pos * (keyLen + 8);
}

static inline char* internalEntryPtr(char* base, int keyLen, int pos) {
    // entries arranged immediately after header: repeated (key[keyLen] + childPage[4] + pad[4])
    return base + sizeof(IndexPageHeader) + pos * (keyLen + 8);
}

RC IndexManager::findLeaf(TableId indexId, const IndexInfo& info, const KeyBytes &key, PageNum &leafPage, std::vector<PageNum>* path) {
    PageNum cur = info.rootPage;
    if (cur < 0) return RC_PAGE_NOT_FOUND; // should not
    while (true) {
        if (path) path->push_back(cur);
        BufferFrame* frame = nullptr;
        RC rc = readPage(indexId, cur, frame);
        if (rc != RC_OK) return rc;
        auto* hdr = reinterpret_cast<IndexPageHeader*>(frame->data);
        if (hdr->nodeType == (uint8_t)IndexNodeType::LEAF) {
            leafPage = cur;
            releasePage(indexId, cur);
            return RC_OK;
        }
        // internal: decide child
        int n = hdr->keyCount;
        int keyLen = info.keyLen;
        int32_t child = hdr->leftMostChild; // default
        bool decided = false;
        for (int i = 0; i < n; ++i) {
            char* entry = internalEntryPtr(frame->data, keyLen, i);
            KeyBytes k(keyLen); std::memcpy(k.bytes.data(), entry, keyLen);
            int cmp = key.compare(k);
            if (cmp < 0) {
                // go to child to the left of this key
                if (i == 0) child = hdr->leftMostChild;
                else child = *reinterpret_cast<int32_t*>(internalEntryPtr(frame->data, keyLen, i-1) + keyLen);
                decided = true;
                break;
            }
        }
        if (!decided) {
            if (n == 0) { releasePage(indexId, cur); return RC_PAGE_NOT_FOUND; }
            child = *reinterpret_cast<int32_t*>(internalEntryPtr(frame->data, keyLen, n-1) + keyLen);
        }
        releasePage(indexId, cur);
        cur = child;
    }
}

RC IndexManager::insertKey(TableId indexId, const IndexInfo &info, const KeyBytes &key, const RID &rid) {
    // 1. 定位叶子
    PageNum leafPage;
    std::vector<PageNum> path;
    RC rc = findLeaf(indexId, info, key, leafPage, &path);
    if (rc != RC_OK) return rc;

    BufferFrame* leafFrame = nullptr;
    rc = readPage(indexId, leafPage, leafFrame);
    if (rc != RC_OK) return rc;
    auto* hdr = reinterpret_cast<IndexPageHeader*>(leafFrame->data);
    int n = hdr->keyCount;
    int keyLen = info.keyLen;

    // 唯一性检查（线性）
    if (info.unique) {
        for (int i = 0; i < n; ++i) {
            char* p = leafEntryPtr(leafFrame->data, keyLen, i);
            if (std::memcmp(p, key.bytes.data(), keyLen) == 0) {
                releasePage(indexId, leafPage);
                return RC_INVALID_OP; // 违反唯一性
            }
        }
    }

    // 2. 查找插入位置
    int pos = 0;
    for (; pos < n; ++pos) {
        char* p = leafEntryPtr(leafFrame->data, keyLen, pos);
        KeyBytes k(keyLen); std::memcpy(k.bytes.data(), p, keyLen);
        if (key.compare(k) < 0) break;
    }

    if (n < hdr->maxKeys) {
        // 直接插入：移动尾部
        for (int i = n; i > pos; --i) {
            char* dst = leafEntryPtr(leafFrame->data, keyLen, i);
            char* src = leafEntryPtr(leafFrame->data, keyLen, i-1);
            std::memmove(dst, src, keyLen + 8);
        }
        char* slot = leafEntryPtr(leafFrame->data, keyLen, pos);
        std::memcpy(slot, key.bytes.data(), keyLen);
        *reinterpret_cast<int32_t*>(slot + keyLen) = rid.pageNum;
        *reinterpret_cast<int32_t*>(slot + keyLen + 4) = rid.slotNum;
        hdr->keyCount++;
        memManager_.markDirty(indexId, leafPage);
        releasePage(indexId, leafPage);
        return RC_OK;
    }

    // 3. 需要分裂
    rc = splitLeafAndInsert(indexId, info, leafFrame, key, rid);
    releasePage(indexId, leafPage);
    return rc;
}

RC IndexManager::splitLeafAndInsert(TableId indexId, const IndexInfo &info, BufferFrame *leafFrame, const KeyBytes &key, const RID &rid) {
    auto* hdr = reinterpret_cast<IndexPageHeader*>(leafFrame->data);
    int keyLen = info.keyLen;
    int n = hdr->keyCount;

    // 临时数组收集所有项
    std::vector<char> tmp((n + 1) * (keyLen + 8));
    for (int i = 0; i < n; ++i) {
        std::memcpy(tmp.data() + i*(keyLen+8), leafEntryPtr(leafFrame->data, keyLen, i), keyLen+8);
    }
    // 找插入位置
    int pos = 0;
    for (; pos < n; ++pos) {
        KeyBytes k(keyLen);
        std::memcpy(k.bytes.data(), tmp.data() + pos*(keyLen+8), keyLen);
        if (key.compare(k) < 0) break;
    }
    // 插入到tmp
    for (int i = n; i > pos; --i) {
        std::memcpy(tmp.data() + i*(keyLen+8), tmp.data() + (i-1)*(keyLen+8), keyLen+8);
    }
    std::memcpy(tmp.data() + pos*(keyLen+8), key.bytes.data(), keyLen);
    *reinterpret_cast<int32_t*>(tmp.data() + pos*(keyLen+8) + keyLen) = rid.pageNum;
    *reinterpret_cast<int32_t*>(tmp.data() + pos*(keyLen+8) + keyLen + 4) = rid.slotNum;

    int splitPoint = (n + 1) / 2; // 右侧数量 = (n+1) - splitPoint

    // 分配新叶子
    BlockNum newBlock;
    RC rc = diskManager_.allocBlock(indexId, newBlock);
    if (rc != RC_OK) return rc;

    // 初始化新叶页
    initNewIndexRoot(indexId, newBlock, hdr->maxKeys, true);

    // 左叶：写入前半部分
    hdr->keyCount = splitPoint;
    for (int i = 0; i < splitPoint; ++i) {
        std::memcpy(leafEntryPtr(leafFrame->data, keyLen, i), tmp.data() + i*(keyLen+8), keyLen+8);
    }
    // 右叶：写入后半部分
    BufferFrame* rightFrame = nullptr;
    rc = readPage(indexId, newBlock, rightFrame);
    if (rc != RC_OK) return rc;
    auto* rHdr = reinterpret_cast<IndexPageHeader*>(rightFrame->data);
    int rightCount = (n + 1) - splitPoint;
    rHdr->keyCount = rightCount;
    for (int i = 0; i < rightCount; ++i) {
        std::memcpy(leafEntryPtr(rightFrame->data, keyLen, i), tmp.data() + (splitPoint + i)*(keyLen+8), keyLen+8);
    }

    // 维护链表和父指针
    rHdr->prevPage = hdr->pageNum;
    rHdr->nextPage = hdr->nextPage;
    hdr->nextPage = newBlock;
    rHdr->parentPage = hdr->parentPage;

    memManager_.markDirty(indexId, hdr->pageNum);
    memManager_.markDirty(indexId, newBlock);

    // 上提的键（右叶的最小键）
    KeyBytes upKey(keyLen);
    if (rightCount > 0) std::memcpy(upKey.bytes.data(), leafEntryPtr(rightFrame->data, keyLen, 0), keyLen);

    // 插入父节点
    rc = insertIntoParent(indexId, info, hdr->pageNum, upKey, newBlock);

    releasePage(indexId, newBlock);
    return rc;
}

RC IndexManager::insertIntoParent(TableId indexId, const IndexInfo &info, PageNum left, const KeyBytes &upKey, PageNum right) {
    // 读取左节点以获父页
    BufferFrame* leftFrame = nullptr;
    RC rc = readPage(indexId, left, leftFrame);
    if (rc != RC_OK) return rc;
    auto* leftHdr = reinterpret_cast<IndexPageHeader*>(leftFrame->data);
    PageNum parent = leftHdr->parentPage;

    if (parent == -1) {
        // 创建新根
        BlockNum newRoot;
        rc = diskManager_.allocBlock(indexId, newRoot);
        if (rc != RC_OK) { releasePage(indexId, left); return rc; }
        initNewIndexRoot(indexId, newRoot, calcMaxKeys(info.keyLen), false);

        BufferFrame* r = nullptr;
        rc = readPage(indexId, newRoot, r);
        if (rc != RC_OK) { releasePage(indexId, left); return rc; }
        auto* rh = reinterpret_cast<IndexPageHeader*>(r->data);
        rh->leftMostChild = left;
        rh->keyCount = 1;
        // 写第一个key和右孩子
        char* e0 = internalEntryPtr(r->data, info.keyLen, 0);
        std::memcpy(e0, upKey.bytes.data(), info.keyLen);
        *reinterpret_cast<int32_t*>(e0 + info.keyLen) = right;
        // 更新孩子父指针
        leftHdr->parentPage = newRoot;
        BufferFrame* rightFrame = nullptr; readPage(indexId, right, rightFrame);
        auto* rightHdr = reinterpret_cast<IndexPageHeader*>(rightFrame->data);
        rightHdr->parentPage = newRoot;
        memManager_.markDirty(indexId, newRoot);
        memManager_.markDirty(indexId, left);
        memManager_.markDirty(indexId, right);
        releasePage(indexId, right);
        releasePage(indexId, newRoot);

        // 更新索引根信息
        IndexInfo updated = info;
        updated.rootPage = newRoot;
        updated.height = std::max(1, info.height) + 1;
        dataDict_.updateIndexInfo(updated);
        releasePage(indexId, left);
        return RC_OK;
    }

    // 将(upKey, right)插入到父节点的合适位置
    BufferFrame* pFrame = nullptr;
    rc = readPage(indexId, parent, pFrame);
    if (rc != RC_OK) { releasePage(indexId, left); return rc; }
    auto* ph = reinterpret_cast<IndexPageHeader*>(pFrame->data);
    int n = ph->keyCount;
    int keyLen = info.keyLen;

    // 找left在父中的位置：需要比较孩子指针
    int insertPos = 0;
    // 首先检查left是否是leftMostChild
    if (ph->leftMostChild == left) insertPos = 0;
    else {
        for (int i = 0; i < n; ++i) {
            int32_t child = *reinterpret_cast<int32_t*>(internalEntryPtr(pFrame->data, keyLen, i) + keyLen);
            if (child == left) { insertPos = i + 1; break; }
        }
    }

    if (n < ph->maxKeys) {
        // 向后移动entry为新键腾出空间
        for (int i = n; i > insertPos; --i) {
            char* dst = internalEntryPtr(pFrame->data, keyLen, i);
            char* src = internalEntryPtr(pFrame->data, keyLen, i-1);
            std::memmove(dst, src, keyLen + 8);
        }
        char* slot = internalEntryPtr(pFrame->data, keyLen, insertPos);
        std::memcpy(slot, upKey.bytes.data(), keyLen);
        *reinterpret_cast<int32_t*>(slot + keyLen) = right;
        ph->keyCount++;
        memManager_.markDirty(indexId, parent);
        releasePage(indexId, parent);
        releasePage(indexId, left);
        return RC_OK;
    }

    // 需要分裂父节点
    rc = splitInternalAndInsert(indexId, info, pFrame, upKey, right);
    releasePage(indexId, parent);
    releasePage(indexId, left);
    return rc;
}

RC IndexManager::splitInternalAndInsert(TableId indexId, const IndexInfo &info, BufferFrame *internalFrame, const KeyBytes &upKey, PageNum right) {
    auto* hdr = reinterpret_cast<IndexPageHeader*>(internalFrame->data);
    int n = hdr->keyCount;
    int keyLen = info.keyLen;

    // 收集现有keys和children（children通过leftMost + 每个entry的右孩子）
    std::vector<char> tmpKeys((n + 1) * (keyLen + 8));
    int32_t leftMost = hdr->leftMostChild;
    for (int i = 0; i < n; ++i) {
        std::memcpy(tmpKeys.data() + i*(keyLen+8), internalEntryPtr(internalFrame->data, keyLen, i), keyLen+8);
    }

    // 确定插入位置：根据upKey顺序
    int pos = 0;
    for (; pos < n; ++pos) {
        KeyBytes k(keyLen); std::memcpy(k.bytes.data(), tmpKeys.data() + pos*(keyLen+8), keyLen);
        if (upKey.compare(k) < 0) break;
    }
    for (int i = n; i > pos; --i) {
        std::memcpy(tmpKeys.data() + i*(keyLen+8), tmpKeys.data() + (i-1)*(keyLen+8), keyLen+8);
    }
    // 插入(upKey,right)
    std::memcpy(tmpKeys.data() + pos*(keyLen+8), upKey.bytes.data(), keyLen);
    *reinterpret_cast<int32_t*>(tmpKeys.data() + pos*(keyLen+8) + keyLen) = right;

    int total = n + 1;
    int mid = total / 2; // 提升中间键 [mid]

    // 新右内部页
    BlockNum newBlock; RC rc = diskManager_.allocBlock(indexId, newBlock); if (rc != RC_OK) return rc;
    initNewIndexRoot(indexId, newBlock, hdr->maxKeys, false);

    // 左页：保留[0..mid-1]
    hdr->keyCount = mid;
    for (int i = 0; i < mid; ++i) {
        std::memcpy(internalEntryPtr(internalFrame->data, keyLen, i), tmpKeys.data() + i*(keyLen+8), keyLen+8);
    }
    // 计算右页的leftMostChild = c_mid
    int32_t c_mid = (mid == 0) ? leftMost : *reinterpret_cast<int32_t*>(tmpKeys.data() + (mid-1)*(keyLen+8) + keyLen);

    // 提升键 = mid项的key
    KeyBytes promote(keyLen); std::memcpy(promote.bytes.data(), tmpKeys.data() + mid*(keyLen+8), keyLen);

    // 右页：写入[mid+1..end)
    BufferFrame* r = nullptr; rc = readPage(indexId, newBlock, r); if (rc != RC_OK) return rc;
    auto* rh = reinterpret_cast<IndexPageHeader*>(r->data);
    int rightCount = total - (mid + 1);
    rh->keyCount = rightCount;
    rh->leftMostChild = c_mid;
    for (int i = 0; i < rightCount; ++i) {
        std::memcpy(internalEntryPtr(r->data, keyLen, i), tmpKeys.data() + (mid+1+i)*(keyLen+8), keyLen+8);
    }

    memManager_.markDirty(indexId, hdr->pageNum);
    memManager_.markDirty(indexId, newBlock);

    // 插入到父
    rc = insertIntoParent(indexId, info, hdr->pageNum, promote, newBlock);
    releasePage(indexId, newBlock);
    return rc;
}

RC IndexManager::deleteKey(TableId indexId, const IndexInfo &info, const KeyBytes &key, const RID &rid) {
    // 找叶子
    PageNum leaf;
    RC rc = findLeaf(indexId, info, key, leaf, nullptr);
    if (rc != RC_OK) return rc;
    BufferFrame* frame = nullptr; rc = readPage(indexId, leaf, frame); if (rc != RC_OK) return rc;
    auto* hdr = reinterpret_cast<IndexPageHeader*>(frame->data);
    int keyLen = info.keyLen; int n = hdr->keyCount;

    // 线性查找匹配的key+rid
    int pos = -1;
    for (int i = 0; i < n; ++i) {
        char* p = leafEntryPtr(frame->data, keyLen, i);
        if (std::memcmp(p, key.bytes.data(), keyLen) == 0) {
            int32_t pnum = *reinterpret_cast<int32_t*>(p + keyLen);
            int32_t snum = *reinterpret_cast<int32_t*>(p + keyLen + 4);
            if (pnum == rid.pageNum && snum == rid.slotNum) { pos = i; break; }
        }
    }
    if (pos == -1) { releasePage(indexId, leaf); return RC_SLOT_NOT_FOUND; }

    // 删除：向前覆盖
    for (int i = pos; i < n-1; ++i) {
        char* dst = leafEntryPtr(frame->data, keyLen, i);
        char* src = leafEntryPtr(frame->data, keyLen, i+1);
        std::memmove(dst, src, keyLen + 8);
    }
    hdr->keyCount--;
    memManager_.markDirty(indexId, leaf);

    // 叶子下溢重平衡
    PageNum leafPageNum = leaf;
    releasePage(indexId, leaf);

    return rebalanceAfterDelete(indexId, info, leafPageNum);
}

// ===== 辅助：父子关系操作 =====
int32_t IndexManager::getChildAt(char* parentPageData, int keyLen, int childIndex) const {
    auto* ph = reinterpret_cast<IndexPageHeader*>(parentPageData);
    if (childIndex == 0) return ph->leftMostChild;
    if (childIndex - 1 < ph->keyCount) {
        char* e = internalEntryPtr(parentPageData, keyLen, childIndex - 1);
        return *reinterpret_cast<int32_t*>(e + keyLen);
    }
    return -1;
}

int IndexManager::findChildIndex(char* parentPageData, int keyLen, PageNum childPage) const {
    auto* ph = reinterpret_cast<IndexPageHeader*>(parentPageData);
    if (ph->leftMostChild == childPage) return 0;
    for (int i = 0; i < ph->keyCount; ++i) {
        char* e = internalEntryPtr(parentPageData, keyLen, i);
        int32_t ch = *reinterpret_cast<int32_t*>(e + keyLen);
        if (ch == childPage) return i + 1; // index of child in [0..keyCount]
    }
    return -1;
}

RC IndexManager::updateParentKeyForRightChild(TableId indexId, BufferFrame* parentFrame, int keyLen, int keyPos, BufferFrame* rightChildFrame) {
    auto* rh = reinterpret_cast<IndexPageHeader*>(rightChildFrame->data);
    if (rh->keyCount <= 0) return RC_OK; // nothing to update
    // 取右孩子的最小键（叶：第0项；内部：第0项）
    char* src = (rh->nodeType == (uint8_t)IndexNodeType::LEAF)
                ? leafEntryPtr(rightChildFrame->data, keyLen, 0)
                : internalEntryPtr(rightChildFrame->data, keyLen, 0);
    char* dst = internalEntryPtr(parentFrame->data, keyLen, keyPos);
    std::memcpy(dst, src, keyLen);
    memManager_.markDirty(parentFrame->tableId, parentFrame->pageNum);
    return RC_OK;
}

// ===== New: removeParentEntryAt and internal rebalance exist; add shrinkRootIfNeeded and leaf rebalance =====
RC IndexManager::removeParentEntryAt(TableId indexId, const IndexInfo& info, BufferFrame* parentFrame, int removeKeyPos) {
    auto* ph = reinterpret_cast<IndexPageHeader*>(parentFrame->data);
    int n = ph->keyCount;
    int keyLen = info.keyLen;
    if (removeKeyPos < 0 || removeKeyPos >= n) return RC_INVALID_ARG;

    // 将[removeKeyPos+1..n-1]左移一位
    for (int i = removeKeyPos; i < n - 1; ++i) {
        char* dst = internalEntryPtr(parentFrame->data, keyLen, i);
        char* src = internalEntryPtr(parentFrame->data, keyLen, i + 1);
        std::memmove(dst, src, keyLen + 8);
    }
    ph->keyCount--;
    memManager_.markDirty(indexId, ph->pageNum);

    // 根可能需要收缩
    if (ph->parentPage == -1) {
        return shrinkRootIfNeeded(indexId, info, parentFrame);
    }

    // 非根：若下溢则递归重平衡
    int minKeys = minKeysForNode(ph->maxKeys);
    if (ph->keyCount < minKeys) {
        PageNum pnum = ph->pageNum;
        // 递归处理
        return rebalanceInternalAfterDelete(indexId, info, pnum);
    }
    return RC_OK;
}

// 收缩根：若根是内部节点且没有键且只有一个孩子，则让孩子成为新根
RC IndexManager::shrinkRootIfNeeded(TableId indexId, const IndexInfo& infoIn, BufferFrame* rootFrame) {
    auto* rh = reinterpret_cast<IndexPageHeader*>(rootFrame->data);
    IndexInfo info = infoIn; // 本地副本以便更新

    if (rh->parentPage != -1) return RC_OK; // 非根

    if (rh->nodeType == (uint8_t)IndexNodeType::INTERNAL && rh->keyCount == 0) {
        // 仅一个孩子
        PageNum child = rh->leftMostChild;
        if (child >= 0) {
            BufferFrame* cf = nullptr; RC rc = readPage(indexId, child, cf); if (rc != RC_OK) return rc;
            auto* ch = reinterpret_cast<IndexPageHeader*>(cf->data);
            ch->parentPage = -1;
            memManager_.markDirty(indexId, child);
            releasePage(indexId, child);
            info.rootPage = child;
            info.height = std::max(1, info.height - 1);
            dataDict_.updateIndexInfo(info);
        }
    }
    return RC_OK;
}

// 叶子删除后的借位/合并
RC IndexManager::rebalanceAfterDelete(TableId indexId, const IndexInfo &info, PageNum leafPage) {
    BufferFrame* leaf = nullptr; RC rc = readPage(indexId, leafPage, leaf); if (rc != RC_OK) return rc;
    auto* lh = reinterpret_cast<IndexPageHeader*>(leaf->data);
    int keyLen = info.keyLen;

    // 若是根或未下溢，则直接返回
    int minKeys = minKeysForNode(lh->maxKeys);
    if (lh->parentPage == -1 || lh->keyCount >= minKeys) { releasePage(indexId, leafPage); return RC_OK; }

    // 加载父
    BufferFrame* parent = nullptr; rc = readPage(indexId, lh->parentPage, parent); if (rc != RC_OK) { releasePage(indexId, leafPage); return rc; }
    int childIndex = findChildIndex(parent->data, keyLen, leafPage);
    if (childIndex < 0) { releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage); return RC_PAGE_NOT_FOUND; }

    // 尝试向左借
    if (childIndex - 1 >= 0) {
        int32_t leftPage = getChildAt(parent->data, keyLen, childIndex - 1);
        if (leftPage != -1) {
            BufferFrame* left = nullptr; if (readPage(indexId, leftPage, left) == RC_OK) {
                auto* lhdr = reinterpret_cast<IndexPageHeader*>(left->data);
                if (lhdr->nodeType == (uint8_t)IndexNodeType::LEAF && lhdr->keyCount > minKeys) {
                    // 将左兄弟最后一项移到当前页开头
                    int lpos = lhdr->keyCount - 1;
                    // 右移当前页项
                    for (int i = lh->keyCount; i > 0; --i) {
                        std::memmove(leafEntryPtr(leaf->data, keyLen, i), leafEntryPtr(leaf->data, keyLen, i-1), keyLen + 8);
                    }
                    std::memcpy(leafEntryPtr(leaf->data, keyLen, 0), leafEntryPtr(left->data, keyLen, lpos), keyLen + 8);
                    lh->keyCount++;
                    lhdr->keyCount--;
                    // 更新父分隔键为当前页新的最小键
                    char* sepDst = internalEntryPtr(parent->data, keyLen, childIndex - 1);
                    std::memcpy(sepDst, leafEntryPtr(leaf->data, keyLen, 0), keyLen);
                    memManager_.markDirty(indexId, leftPage);
                    memManager_.markDirty(indexId, lh->parentPage);
                    memManager_.markDirty(indexId, leafPage);
                    releasePage(indexId, leftPage); releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage);
                    return RC_OK;
                }
                releasePage(indexId, leftPage);
            }
        }
    }

    // 尝试向右借
    if (childIndex + 1 <= reinterpret_cast<IndexPageHeader*>(parent->data)->keyCount) {
        int32_t rightPage = getChildAt(parent->data, keyLen, childIndex + 1);
        if (rightPage != -1) {
            BufferFrame* right = nullptr; if (readPage(indexId, rightPage, right) == RC_OK) {
                auto* rh = reinterpret_cast<IndexPageHeader*>(right->data);
                if (rh->nodeType == (uint8_t)IndexNodeType::LEAF && rh->keyCount > minKeys) {
                    // 将右兄弟第一项移到当前页末尾
                    char* src = leafEntryPtr(right->data, keyLen, 0);
                    std::memcpy(leafEntryPtr(leaf->data, keyLen, lh->keyCount), src, keyLen + 8);
                    lh->keyCount++;
                    // 右页整体左移一位
                    for (int i = 0; i < rh->keyCount - 1; ++i) {
                        std::memmove(leafEntryPtr(right->data, keyLen, i), leafEntryPtr(right->data, keyLen, i+1), keyLen + 8);
                    }
                    rh->keyCount--;
                    // 更新父分隔键为右页新的最小键
                    char* sepDst = internalEntryPtr(parent->data, keyLen, childIndex);
                    if (rh->keyCount > 0) std::memcpy(sepDst, leafEntryPtr(right->data, keyLen, 0), keyLen);
                    memManager_.markDirty(indexId, rightPage);
                    memManager_.markDirty(indexId, lh->parentPage);
                    memManager_.markDirty(indexId, leafPage);
                    releasePage(indexId, rightPage); releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage);
                    return RC_OK;
                }
                releasePage(indexId, rightPage);
            }
        }
    }

    // 不能借 -> 合并（优先与左合并）
    if (childIndex - 1 >= 0) {
        int32_t leftPage = getChildAt(parent->data, keyLen, childIndex - 1);
        if (leftPage != -1) {
            BufferFrame* left = nullptr; if (readPage(indexId, leftPage, left) == RC_OK) {
                auto* lhdr = reinterpret_cast<IndexPageHeader*>(left->data);
                if (lhdr->nodeType == (uint8_t)IndexNodeType::LEAF) {
                    // 左页吸收当前页
                    for (int i = 0; i < lh->keyCount; ++i) {
                        std::memcpy(leafEntryPtr(left->data, keyLen, lhdr->keyCount + i), leafEntryPtr(leaf->data, keyLen, i), keyLen + 8);
                    }
                    lhdr->keyCount += lh->keyCount;
                    // 维护链表
                    lhdr->nextPage = lh->nextPage;
                    if (lh->nextPage != -1) {
                        BufferFrame* nxt = nullptr; if (readPage(indexId, lh->nextPage, nxt) == RC_OK) {
                            auto* nh = reinterpret_cast<IndexPageHeader*>(nxt->data);
                            nh->prevPage = lhdr->pageNum;
                            memManager_.markDirty(indexId, nh->pageNum);
                            releasePage(indexId, nh->pageNum);
                        }
                    }
                    memManager_.markDirty(indexId, leftPage);
                    // 从父删除分隔键 childIndex-1
                    RC rrc = removeParentEntryAt(indexId, info, parent, childIndex - 1);
                    releasePage(indexId, leftPage); releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage);
                    return rrc;
                }
                releasePage(indexId, leftPage);
            }
        }
    }

    // 与右合并
    if (childIndex + 1 <= reinterpret_cast<IndexPageHeader*>(parent->data)->keyCount) {
        int32_t rightPage = getChildAt(parent->data, keyLen, childIndex + 1);
        if (rightPage != -1) {
            BufferFrame* right = nullptr; if (readPage(indexId, rightPage, right) == RC_OK) {
                auto* rh = reinterpret_cast<IndexPageHeader*>(right->data);
                if (rh->nodeType == (uint8_t)IndexNodeType::LEAF) {
                    // 当前页吸收右页
                    for (int i = 0; i < rh->keyCount; ++i) {
                        std::memcpy(leafEntryPtr(leaf->data, keyLen, lh->keyCount + i), leafEntryPtr(right->data, keyLen, i), keyLen + 8);
                    }
                    lh->keyCount += rh->keyCount;
                    // 维护链表
                    lh->nextPage = rh->nextPage;
                    if (rh->nextPage != -1) {
                        BufferFrame* nxt = nullptr; if (readPage(indexId, rh->nextPage, nxt) == RC_OK) {
                            auto* nh = reinterpret_cast<IndexPageHeader*>(nxt->data);
                            nh->prevPage = lh->pageNum;
                            memManager_.markDirty(indexId, nh->pageNum);
                            releasePage(indexId, nh->pageNum);
                        }
                    }
                    memManager_.markDirty(indexId, leafPage);
                    // 从父删除分隔键 childIndex
                    RC rrc = removeParentEntryAt(indexId, info, parent, childIndex);
                    releasePage(indexId, rightPage); releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage);
                    return rrc;
                }
                releasePage(indexId, rightPage);
            }
        }
    }

    // 默认
    releasePage(indexId, lh->parentPage); releasePage(indexId, leafPage);
    return RC_OK;
}

RC IndexManager::rebalanceInternalAfterDelete(TableId indexId, const IndexInfo& info, PageNum pageNum) {
    BufferFrame* frame = nullptr; RC rc = readPage(indexId, pageNum, frame); if (rc != RC_OK) return rc;
    auto* hdr = reinterpret_cast<IndexPageHeader*>(frame->data);
    int keyLen = info.keyLen;

    // 根处理
    if (hdr->parentPage == -1) {
        RC s = shrinkRootIfNeeded(indexId, info, frame);
        releasePage(indexId, pageNum);
        return s;
    }

    int minKeys = minKeysForNode(hdr->maxKeys);
    if (hdr->keyCount >= minKeys) { releasePage(indexId, pageNum); return RC_OK; }

    // 取父
    BufferFrame* parent = nullptr; rc = readPage(indexId, hdr->parentPage, parent); if (rc != RC_OK) { releasePage(indexId, pageNum); return rc; }
    auto* ph = reinterpret_cast<IndexPageHeader*>(parent->data);
    int childIndex = findChildIndex(parent->data, keyLen, hdr->pageNum);
    if (childIndex < 0) { releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum); return RC_PAGE_NOT_FOUND; }

    // 先尝试向左借
    if (childIndex - 1 >= 0) {
        int32_t leftPage = getChildAt(parent->data, keyLen, childIndex - 1);
        if (leftPage != -1) {
            BufferFrame* left = nullptr; if (readPage(indexId, leftPage, left) == RC_OK) {
                auto* lh = reinterpret_cast<IndexPageHeader*>(left->data);
                if (lh->nodeType == (uint8_t)IndexNodeType::INTERNAL && lh->keyCount > minKeys) {
                    // 左兄弟的最后一个entry下的右孩子
                    int lpos = lh->keyCount - 1;
                    int32_t borrowChild = *reinterpret_cast<int32_t*>(internalEntryPtr(left->data, keyLen, lpos) + keyLen);
                    // 将父分隔键下移到当前页开头，右孩子=当前leftMostChild
                    for (int i = hdr->keyCount; i > 0; --i) {
                        std::memmove(internalEntryPtr(frame->data, keyLen, i), internalEntryPtr(frame->data, keyLen, i-1), keyLen + 8);
                    }
                    char* parentSep = internalEntryPtr(parent->data, keyLen, childIndex - 1);
                    std::memcpy(internalEntryPtr(frame->data, keyLen, 0), parentSep, keyLen);
                    *reinterpret_cast<int32_t*>(internalEntryPtr(frame->data, keyLen, 0) + keyLen) = hdr->leftMostChild;
                    hdr->leftMostChild = borrowChild;
                    hdr->keyCount++;
                    // 父分隔键替换为左兄弟最后一个key
                    char* leftLastKey = internalEntryPtr(left->data, keyLen, lpos);
                    std::memcpy(parentSep, leftLastKey, keyLen);
                    // 删除左兄弟最后一个entry
                    lh->keyCount--;
                    memManager_.markDirty(indexId, leftPage);
                    memManager_.markDirty(indexId, hdr->parentPage);
                    memManager_.markDirty(indexId, pageNum);
                    setChildrenParent(indexId, std::vector<PageNum>{borrowChild}, pageNum);
                    releasePage(indexId, leftPage); releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum);
                    return RC_OK;
                }
                releasePage(indexId, leftPage);
            }
        }
    }

    // 再尝试向右借
    if (childIndex + 1 <= ph->keyCount) {
        int32_t rightPage = getChildAt(parent->data, keyLen, childIndex + 1);
        if (rightPage != -1) {
            BufferFrame* right = nullptr; if (readPage(indexId, rightPage, right) == RC_OK) {
                auto* rh = reinterpret_cast<IndexPageHeader*>(right->data);
                if (rh->nodeType == (uint8_t)IndexNodeType::INTERNAL && rh->keyCount > minKeys) {
                    int32_t moveChild = rh->leftMostChild;
                    char* parentSep = internalEntryPtr(parent->data, keyLen, childIndex);
                    char* dst = internalEntryPtr(frame->data, keyLen, hdr->keyCount);
                    std::memcpy(dst, parentSep, keyLen);
                    *reinterpret_cast<int32_t*>(dst + keyLen) = moveChild;
                    hdr->keyCount++;
                    char* rfirst = internalEntryPtr(right->data, keyLen, 0);
                    int32_t newLeftMost = *reinterpret_cast<int32_t*>(rfirst + keyLen);
                    rh->leftMostChild = newLeftMost;
                    std::memcpy(parentSep, rfirst, keyLen);
                    for (int i = 0; i < rh->keyCount - 1; ++i) {
                        std::memmove(internalEntryPtr(right->data, keyLen, i), internalEntryPtr(right->data, keyLen, i+1), keyLen + 8);
                    }
                    rh->keyCount--;
                    memManager_.markDirty(indexId, rightPage);
                    memManager_.markDirty(indexId, hdr->parentPage);
                    memManager_.markDirty(indexId, pageNum);
                    setChildrenParent(indexId, std::vector<PageNum>{moveChild}, pageNum);
                    releasePage(indexId, rightPage); releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum);
                    return RC_OK;
                }
                releasePage(indexId, rightPage);
            }
        }
    }

    // 借不到则合并（左优先）
    if (childIndex - 1 >= 0) {
        int32_t leftPage = getChildAt(parent->data, keyLen, childIndex - 1);
        if (leftPage != -1) {
            BufferFrame* left = nullptr; if (readPage(indexId, leftPage, left) == RC_OK) {
                auto* lh2 = reinterpret_cast<IndexPageHeader*>(left->data);
                if (lh2->nodeType == (uint8_t)IndexNodeType::INTERNAL) {
                    std::vector<PageNum> movedChildren;
                    movedChildren.reserve(hdr->keyCount + 1);
                    char* parentSep = internalEntryPtr(parent->data, keyLen, childIndex - 1);
                    char* dst = internalEntryPtr(left->data, keyLen, lh2->keyCount);
                    std::memcpy(dst, parentSep, keyLen);
                    *reinterpret_cast<int32_t*>(dst + keyLen) = hdr->leftMostChild;
                    movedChildren.push_back(hdr->leftMostChild);
                    for (int i = 0; i < hdr->keyCount; ++i) {
                        char* src = internalEntryPtr(frame->data, keyLen, i);
                        std::memcpy(internalEntryPtr(left->data, keyLen, lh2->keyCount + 1 + i), src, keyLen + 8);
                        int32_t childR = *reinterpret_cast<int32_t*>(src + keyLen);
                        movedChildren.push_back(childR);
                    }
                    lh2->keyCount += 1 + hdr->keyCount;
                    memManager_.markDirty(indexId, leftPage);
                    setChildrenParent(indexId, movedChildren, leftPage);
                    RC rrc = removeParentEntryAt(indexId, info, parent, childIndex - 1);
                    releasePage(indexId, leftPage); releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum);
                    return rrc;
                }
                releasePage(indexId, leftPage);
            }
        }
    }

    if (childIndex + 1 <= ph->keyCount) {
        int32_t rightPage = getChildAt(parent->data, keyLen, childIndex + 1);
        if (rightPage != -1) {
            BufferFrame* right = nullptr; if (readPage(indexId, rightPage, right) == RC_OK) {
                auto* rh2 = reinterpret_cast<IndexPageHeader*>(right->data);
                if (rh2->nodeType == (uint8_t)IndexNodeType::INTERNAL) {
                    std::vector<PageNum> movedChildren;
                    movedChildren.reserve(rh2->keyCount + 1);
                    char* parentSep = internalEntryPtr(parent->data, keyLen, childIndex);
                    char* dst = internalEntryPtr(frame->data, keyLen, hdr->keyCount);
                    std::memcpy(dst, parentSep, keyLen);
                    *reinterpret_cast<int32_t*>(dst + keyLen) = rh2->leftMostChild;
                    movedChildren.push_back(rh2->leftMostChild);
                    for (int i = 0; i < rh2->keyCount; ++i) {
                        char* src = internalEntryPtr(right->data, keyLen, i);
                        std::memcpy(internalEntryPtr(frame->data, keyLen, hdr->keyCount + 1 + i), src, keyLen + 8);
                        int32_t childR = *reinterpret_cast<int32_t*>(src + keyLen);
                        movedChildren.push_back(childR);
                    }
                    hdr->keyCount += 1 + rh2->keyCount;
                    memManager_.markDirty(indexId, pageNum);
                    setChildrenParent(indexId, movedChildren, pageNum);
                    RC rrc = removeParentEntryAt(indexId, info, parent, childIndex);
                    releasePage(indexId, rightPage); releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum);
                    return rrc;
                }
                releasePage(indexId, rightPage);
            }
        }
    }

    // 默认
    releasePage(indexId, hdr->parentPage); releasePage(indexId, pageNum);
    return RC_OK;
}

RC IndexManager::setChildrenParent(TableId indexId, const std::vector<PageNum>& children, PageNum newParent) {
    for (PageNum c : children) {
        if (c < 0) continue;
        BufferFrame* f = nullptr; RC rc = readPage(indexId, c, f); if (rc != RC_OK) return rc;
        auto* h = reinterpret_cast<IndexPageHeader*>(f->data);
        h->parentPage = newParent;
        memManager_.markDirty(indexId, c);
        releasePage(indexId, c);
    }
    return RC_OK;
}

RC IndexManager::onRecordInserted(const TableInfo &table, const char *data, int len, const RID &rid) {
    // 针对该表的所有索引插入键
    std::vector<IndexInfo> idxs; dataDict_.listIndexesForTable(table.tableId, idxs);
    for (auto& idx : idxs) {
        KeyBytes kb = extractKey(data, len, idx.keyType, idx.keyLen);
        insertKey(idx.indexId, idx, kb, rid);
    }
    return RC_OK;
}

RC IndexManager::onRecordDeleted(const TableInfo &table, const char *data, int len, const RID &rid) {
    // 针对该表的所有索引删除键
    std::vector<IndexInfo> idxs; dataDict_.listIndexesForTable(table.tableId, idxs);
    for (auto& idx : idxs) {
        KeyBytes kb = extractKey(data, len, idx.keyType, idx.keyLen);
        deleteKey(idx.indexId, idx, kb, rid);
    }
    return RC_OK;
}

RC IndexManager::showIndex(const char *indexName) {
    IndexInfo idx; RC rc = dataDict_.findIndex(indexName, idx);
    if (rc != RC_OK) { std::cout << "Index not found: " << indexName << std::endl; return rc; }

    // 确保索引文件已打开
    diskManager_.openTableFile(idx.indexId);

    TableFileHeader hdr; rc = diskManager_.readTableFileHeader(idx.indexId, hdr);
    if (rc != RC_OK) return rc;

    std::cout << "Index: " << idx.indexName << ", Table: " << idx.tableName
              << ", Column: " << idx.columnName << ", Root: " << idx.rootPage
              << ", Height: " << idx.height << ", UsedBlocks: " << hdr.usedBlocks << std::endl;

    char buf[BLOCK_SIZE];
    for (BlockNum b = 0; b < hdr.usedBlocks; ++b) {
        RC rr = diskManager_.readBlock(idx.indexId, b, buf);
        if (rr != RC_OK) continue;
        auto* ph = reinterpret_cast<IndexPageHeader*>(buf);
        if (ph->keyCount <= 0 && ph->pageNum != idx.rootPage) continue;
        std::cout << "  Page #" << b << " type=" << ((ph->nodeType==(uint8_t)IndexNodeType::LEAF)?"LEAF":"INTERNAL")
                  << " prev=" << ph->prevPage << " next=" << ph->nextPage
                  << " keys=" << ph->keyCount << "/" << ph->maxKeys << std::endl;
        int keyLen = idx.keyLen;
        int show = std::min<int>(ph->keyCount, 6);
        for (int i = 0; i < show; ++i) {
            if (ph->nodeType == (uint8_t)IndexNodeType::LEAF) {
                char* p = leafEntryPtr(buf, keyLen, (i<3?i:(ph->keyCount - (show - i))));
                int32_t page = *reinterpret_cast<int32_t*>(p + keyLen);
                int32_t slot = *reinterpret_cast<int32_t*>(p + keyLen + 4);
                int32_t keyPreview = 0; std::memcpy(&keyPreview, p, std::min(4, keyLen));
                std::cout << "    [" << i << "] key~=" << keyPreview << " -> (" << page << "," << slot << ")" << std::endl;
            } else {
                char* e = internalEntryPtr(buf, keyLen, (i<3?i:(ph->keyCount - (show - i))));
                int32_t child = *reinterpret_cast<int32_t*>(e + keyLen);
                int32_t keyPreview = 0; std::memcpy(&keyPreview, e, std::min(4, keyLen));
                std::cout << "    [" << i << "] key~=" << keyPreview << " -> child=" << child << std::endl;
            }
        }
    }

    return RC_OK;
}
