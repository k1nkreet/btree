#pragma once
#include "btree_node.h"
#include <string>
#include <stdint.h>


class BTreeFS {
public:
explicit BTreeFS(const std::string &filename);
    BTreeFS(const std::string &filename, int order);
    BTreeNode openNode(uint64_t ref) const;
    void saveNode(const BTreeNode &node);
    BTreeNode allocNode(bool is_leaf);
    int order() const;
    uint64_t rootRef() const;
    void setRootRef(uint64_t root);
    uint64_t treeSize() const;
    void setTreeSize(uint64_t tree_size);
    int treeHeight() const;
    void setTreeHeight(int tree_height);
    uint32_t pageSize() const;
    uint64_t pagesAllocated() const;
    ~BTreeFS();
    static const uint32_t MAX_PAGE_SIZE;
private:
    void readHeader();
    void writeHeader();
    bool refIsValid(uint64_t ref) const;
    uint32_t headerLength() const;
    std::string _filename;
    int _order;
    int _fd;
    uint64_t _root_ref;
    uint64_t _tree_size;
    int _tree_height;
    uint32_t _page_size;
    uint64_t _pages_allocated;
    
};
