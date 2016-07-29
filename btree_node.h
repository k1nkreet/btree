#pragma once
#include <stdint.h>
#include <map>
#include <vector>

class BTreeNode {
public:
    BTreeNode(int order, uint64_t ref, bool is_leaf);
    BTreeNode(BTreeNode &&);
    friend bool operator==(const BTreeNode &, const BTreeNode &);
    void put(int key);
    void put(const BTreeNode &node);
    uint64_t next(int key) const;
    std::map<int, uint64_t> keys() const;
    void removeKey(int key);
    int order() const;
    int keysNum() const;
    bool isFull() const;
    uint64_t ref() const;
    bool isLeaf() const;
    uint64_t sentinel() const;
    int minKey() const;
    int maxKey() const;
    bool contains(int key) const;

    uint64_t nextChild(int key) const;
    uint64_t prevChild(int key) const;
    void setIsLeaf(bool is_leaf);
    void setSentinel(uint64_t child);
    void setKeysNum(int keys_num);
    void addChild(int key, uint64_t child);
    int serialize(uint8_t *page) const;
    static BTreeNode deserialize(const uint8_t *page, int page_size);
    static int maxNodeSerializationSize(int order);
private:
    int _order;
    int _keys_num;
    uint64_t _ref;
    bool _is_leaf;
    uint64_t _sentinel;
    std::map<int, uint64_t> _keys;
};


