#include "btree_node.h"

#include <algorithm>
#include <iterator>
#include <iostream>

#include <string.h>

BTreeNode::BTreeNode(int order, uint64_t ref, bool is_leaf):
    _order(order),
    _keys_num(0),
    _ref(ref),
    _is_leaf(is_leaf),
    _sentinel(0),
    _keys() { }

BTreeNode::BTreeNode(BTreeNode &&that): 
    _order(that._order),
    _keys_num(that._keys_num),
    _ref(that._ref),
    _is_leaf(that._is_leaf),
    _sentinel(that._sentinel)
{
    std::swap(_keys, that._keys);
}

bool operator==(const BTreeNode &left, const BTreeNode &right) {
    if (left._order != right._order) return false;
    if (left._keys_num != right._keys_num) return false;
    if (left._ref != right._ref) return false;
    if (left._is_leaf != right._is_leaf) return false;
    if (left._sentinel != right._sentinel) return false;
    return left._keys == right._keys;
}


uint64_t BTreeNode::next(int key) const {
    auto it = _keys.upper_bound(key);
    if (it != _keys.begin()) {
        return (--it)->second;
    }
    return _sentinel;
}

void BTreeNode::put(int key) {
    if (_keys.find(key) != _keys.end()) {
        throw std::logic_error("Key already exists");
    }
    
    _keys[key] = 0;
    ++_keys_num;
}

void BTreeNode::put(const BTreeNode &node) {
    _keys[node.minKey()] = node.ref();
    ++_keys_num;
}

std::map<int, uint64_t> BTreeNode::keys() const {
    return _keys;
}

void BTreeNode::removeKey(int key) {
    auto it = _keys.find(key);
    if (it == _keys.end())
        throw std::logic_error("Invalid key");
    _keys.erase(it);
    --_keys_num;
}

int BTreeNode::order() const {
    return _order;
}

int BTreeNode::keysNum() const {
    return _keys_num;
}

bool BTreeNode::isFull() const {
    return _keys_num > _order;
}

uint64_t BTreeNode::ref() const {
    return _ref;
}

bool BTreeNode::isLeaf() const {
    return _is_leaf;
}

uint64_t BTreeNode::sentinel() const {
    return _sentinel;
}

int BTreeNode::minKey() const {
    if (_keys_num > 0)
        return _keys.begin()->first;
    else 
        throw std::logic_error("Ivalid node operation");
}

int BTreeNode::maxKey() const {
    if (_keys_num > 0) 
        return _keys.rbegin()->first;
    else
        throw std::logic_error("Invalid node operation");
}

bool BTreeNode::contains(int key) const {
    return _keys.find(key) != _keys.end();
}

void BTreeNode::setIsLeaf(bool is_leaf) {
    _is_leaf = is_leaf;
}

int BTreeNode::serialize(uint8_t *page) const {
    int offset = 0;
    memcpy(page + offset, &_order, sizeof(_order));
    offset += sizeof(_order);
    memcpy(page + offset, &_keys_num, sizeof(_keys_num));
    offset += sizeof(_order);
    memcpy(page + offset, &_ref, sizeof(_ref));
    offset += sizeof(_ref);
    memcpy(page + offset, &_is_leaf, sizeof(_is_leaf));
    offset += sizeof(_is_leaf);
    memcpy(page + offset, &_sentinel, sizeof(_sentinel));
    offset += sizeof(_sentinel);
    for (std::pair<int, uint64_t> pair: _keys) {
        memcpy(page + offset, &pair.first, sizeof(pair.first));
        offset += sizeof(pair.first);
        memcpy(page + offset, &pair.second, sizeof(pair.second));
        offset += sizeof(pair.second);
    }
    return offset;
}

BTreeNode BTreeNode::deserialize(const uint8_t *page, int page_size) {
    int order;
    int keys_num;
    uint64_t ref;
    bool is_leaf;
    uint64_t sentinel;
    int offset = 0;
    memcpy(&order, page + offset, sizeof(order));
    offset += sizeof(order);
    memcpy(&keys_num, page + offset, sizeof(keys_num));
    offset += sizeof(keys_num);
    memcpy(&ref, page + offset, sizeof(ref));
    offset += sizeof(ref);
    memcpy(&is_leaf, page + offset, sizeof(is_leaf));
    offset += sizeof(is_leaf);
    memcpy(&sentinel, page + offset, sizeof(sentinel));
    offset += sizeof(sentinel);
    BTreeNode node(order, ref, is_leaf);
    node.setSentinel(sentinel);
    node.setKeysNum(keys_num);
    for (int i = 0; i < keys_num; i++) {
        int key;
        memcpy(&key, page + offset, sizeof(key));
        offset += sizeof(key);
        uint64_t child;
        memcpy(&child, page + offset, sizeof(child));
        offset += sizeof(child);
        node.addChild(key, child);
    }

    if (offset > page_size)
        throw std::logic_error("Deserialization error");
    return node;
}

int BTreeNode::maxNodeSerializationSize(int order) {
    int size = sizeof(int);                     //order
    size += sizeof(int);                        //keys_num
    size += sizeof(uint64_t);                   //ref
    size += sizeof(bool);                       //is_leaf
    size += sizeof(uint64_t);                   //sentinel
    size += (order + 1) * sizeof(int);                //keys
    size += (order + 1) * sizeof(uint64_t);           //children
    return size;
}

void BTreeNode::setSentinel(uint64_t sentinel) {
    _sentinel = sentinel;
}

void BTreeNode::addChild(int key, uint64_t child) {
    _keys[key] = child;
}

void BTreeNode::setKeysNum(int keys_num) {
    _keys_num = keys_num;
}

uint64_t BTreeNode::nextChild(int key) const {
    auto it = _keys.find(key);
    if (it == _keys.end())
        return 0;
    if (std::distance(it, _keys.end()) == 1)
        return 0;
    return (++it)->second;
}

uint64_t BTreeNode::prevChild(int key) const {    
    auto it = _keys.find(key);
    if (it == _keys.end() || it == _keys.begin())
        return 0;
    return (--it)->second;
}
