#include "btree_fs.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include <stdexcept>
#include <iostream>


BTreeFS::BTreeFS(const std::string &filename):
    _filename(filename) {
    if (access(filename.c_str(), F_OK) == -1) {
        throw std::logic_error("File not found " + filename);
    }
    _fd = open(_filename.c_str(), O_RDWR | O_EXCL, 0644);
    if (_fd == -1) {
        throw std::logic_error("Could not open " + filename);
    }
    readHeader();
}

BTreeFS::BTreeFS(const std::string &filename, int order) :
    _filename(filename),
    _order(order),
    _tree_size(0),
    _tree_height(0),
    _pages_allocated(0) {
    _page_size = BTreeNode::maxNodeSerializationSize(_order);
    if (_page_size > MAX_PAGE_SIZE) {
        throw std::logic_error("Page size is too big. Try to decrease tree order");
    }
    _fd = open(_filename.c_str(), O_CREAT | O_RDWR, 0644);
    if (_fd == -1) {
        throw std::logic_error("Could not open " + filename);
    }    
}

BTreeNode BTreeFS::openNode(uint64_t ref) const {
    if (!refIsValid(ref)) {
        throw std::logic_error("Invalid reference");
    }
    uint8_t page[MAX_PAGE_SIZE];
    ssize_t bytes_read = pread(_fd, page, _page_size, ref);
    if (bytes_read != _page_size) {
        throw std::logic_error("Could not read page");
    }
    BTreeNode node = BTreeNode::deserialize(page, _page_size);
    return node;
}

void BTreeFS::saveNode(const BTreeNode &node) {
    uint8_t page[MAX_PAGE_SIZE];
    node.serialize(page);
    ssize_t bytes_written = pwrite(_fd, page, _page_size, node.ref());
    if (bytes_written != _page_size) {
        throw std::logic_error("Could not write page");
    }
}

BTreeNode BTreeFS::allocNode(bool is_leaf) {
    uint64_t ref = headerLength() + _pages_allocated * _page_size;
    uint8_t page[MAX_PAGE_SIZE];
    memset(page, 0, _page_size);
    ssize_t bytes_written = pwrite(_fd, page, _page_size, ref);
    if (bytes_written != _page_size) {
        throw std::logic_error("Could not allocate page");
    }
    ++_pages_allocated;
    return BTreeNode(_order, ref, is_leaf);
}

int BTreeFS::order() const {
    return _order;
}

uint64_t BTreeFS::rootRef() const {
    return _root_ref;
}

void BTreeFS::setRootRef(uint64_t root) {
    _root_ref = root;
}

uint64_t BTreeFS::treeSize() const {
    return _tree_size;
}

void BTreeFS::setTreeSize(uint64_t tree_size) {
    _tree_size = tree_size;
}

int BTreeFS::treeHeight() const {
    return _tree_height;
}

void BTreeFS::setTreeHeight(int tree_height) {
    _tree_height = tree_height;
}

uint32_t BTreeFS::pageSize() const {
    return _page_size;
}

uint64_t BTreeFS::pagesAllocated() const {
    return _pages_allocated;
}

void BTreeFS::writeHeader() {
    uint64_t offset = 0;
    if (pwrite(_fd, &_page_size, sizeof(_page_size), offset) != sizeof(_page_size)) {
        throw std::logic_error("Error during FS settings write");
    }
    offset += sizeof(_page_size);
    if (pwrite(_fd, &_pages_allocated, sizeof(_pages_allocated), offset) != sizeof(_pages_allocated)) {
        throw std::logic_error("Error during FS settings write");
    }
    offset += sizeof(_pages_allocated);
    if (pwrite(_fd, &_root_ref, sizeof(_root_ref), offset) != sizeof(_root_ref)) {
        throw std::logic_error("Error during FS settings write");
    }
    offset += sizeof(_root_ref);
    if (pwrite(_fd, &_tree_size, sizeof(_tree_size), offset) != sizeof(_tree_size)) {
        throw std::logic_error("Error during FS settings write");
    }
    offset += sizeof(_tree_size);    
    if (pwrite(_fd, &_tree_height, sizeof(_tree_height), offset) != sizeof(_tree_height)) {
        throw std::logic_error("Error during FS settings write");
    }
    offset += sizeof(_tree_height);
}

void BTreeFS::readHeader() {
    uint64_t offset = 0;
    if (pread(_fd, &_page_size, sizeof(_page_size), offset) != sizeof(_page_size)) {
        throw std::logic_error("Error during FS settings read");
    }
    offset += sizeof(_page_size);
    if (pread(_fd, &_pages_allocated, sizeof(_pages_allocated), offset) != sizeof(_pages_allocated)) {
        throw std::logic_error("Error during FS settings read");
    }
    offset += sizeof(_pages_allocated);
    if (pread(_fd, &_root_ref, sizeof(_root_ref), offset) != sizeof(_root_ref)) {
        throw std::logic_error("Error during FS settings read");
    }
    offset += sizeof(_root_ref);    
    if (pread(_fd, &_tree_size, sizeof(_tree_size), offset) != sizeof(_tree_size)) {
        throw std::logic_error("Error during FS settings read");
    }
    offset += sizeof(_tree_size);
    if (pread(_fd, &_tree_height, sizeof(_tree_height), offset) != sizeof(_tree_height)) {
        throw std::logic_error("Error during FS settings read");
    }
    offset += sizeof(_tree_height);
}

bool BTreeFS::refIsValid(uint64_t ref) const {
    uint32_t header_len = headerLength();
    if (ref < header_len) return false;
    if (ref > header_len + _pages_allocated * _page_size) return false;
    return (ref - header_len) % _page_size == 0;
}


const uint32_t BTreeFS::MAX_PAGE_SIZE = 32768;

uint32_t BTreeFS::headerLength() const {
    uint32_t length = sizeof(_page_size);
    length += sizeof(_pages_allocated);
    length += sizeof(_root_ref);
    length += sizeof(_tree_size);
    length += sizeof(_tree_height);
    return length;
}

BTreeFS::~BTreeFS() {
    try {
        writeHeader();
    }
    catch (const std::exception &e) {
        std::cout << e.what() <<std::endl;
    }
    
    close(_fd);
}
