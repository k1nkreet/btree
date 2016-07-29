#pragma once
#include <string>
#include "btree_fs.h"
#include "btree_node.h"

class BTree {
public:
    BTree(const std::string &filename, int order);
    BTree(const std::string &filename);
    void put(int key);
    void remove(int key);
    bool contains(int key) const;
    uint64_t size() const;
    int height() const;
    class iterator;
    iterator begin() const;
    iterator end() const;
    //debug fucntions
    bool checkValid() const;
    void print() const;
    ~BTree();
private:
    friend class iterator;
    void insert(BTreeNode &node, int key);
    BTreeNode split(BTreeNode &);
    void merge(BTreeNode &, const BTreeNode &);
    void remove(BTreeNode &node, int key);
    bool contains(const BTreeNode &node, int key) const;
    int findNext(int key) const;
    int findNext(const BTreeNode &node, int key) const;
    int minKey() const;
    int minKey(const BTreeNode &node) const;
    void print(const BTreeNode &node, int level) const;
    void balanceSentinel(BTreeNode &node);
    void balanceWithLeftNode(BTreeNode &node, BTreeNode &right);
    void balanceWithRightNode(BTreeNode &node, BTreeNode &next);
    bool checkValid(const BTreeNode &node, int height) const;
    std::string _filename;
    int _order;
    int _height;
    uint64_t _size;    
    BTreeFS _vfs;
    uint64_t _root_ref;
};

class BTree::iterator {
    friend class BTree;
private:
    iterator(const BTree *tree, int key, bool is_end);
public:
    iterator& operator++();
    friend bool operator==(const iterator &, const iterator &);
    friend bool operator!=(const iterator &, const iterator &);
    int operator*() const;
private:
    const BTree *_tree;
    int _key;
    bool _is_end;
};
