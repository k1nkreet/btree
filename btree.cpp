#include "btree.h"
#include <iostream>

BTree::BTree(const std::string &filename, int order):
    _filename(filename),
    _order(order),
    _height(0),
    _size(0),
    _vfs(filename, order) 
{
    BTreeNode root = _vfs.allocNode(true);
    _root_ref = root.ref();
    _vfs.setRootRef(_root_ref);
    _vfs.saveNode(root);
}

BTree::BTree(const std::string &filename):
    _filename(filename),
    _vfs(filename),
    _root_ref(_vfs.rootRef())
{
    _size = _vfs.treeSize();
    _height = _vfs.treeHeight();
    _order = _vfs.order();
}

void BTree::put(int key) {
    BTreeNode root = _vfs.openNode(_root_ref);
    insert(root, key);
    _vfs.setTreeSize(++_size);

    //split root if needs
    if (root.isFull()) {
        BTreeNode node = split(root);
        BTreeNode new_root = _vfs.allocNode(false);
        new_root.put(node);
        new_root.setSentinel(root.ref());
        _vfs.setTreeHeight(++_height);
        _vfs.saveNode(new_root);
        _root_ref = new_root.ref();
        _vfs.setRootRef(_root_ref);
    }
}

void BTree::insert(BTreeNode &node, int key) {
    if (node.isLeaf()) {
        node.put(key);
        _vfs.saveNode(node);
    }
    else {
        BTreeNode next = _vfs.openNode(node.next(key));
        insert(next, key);
        if (next.isFull()) {
            BTreeNode new_node = split(next);
            node.put(new_node);
            _vfs.saveNode(node);
        }
    }
}

BTreeNode BTree::split(BTreeNode &node) {
    BTreeNode new_node = _vfs.allocNode(node.isLeaf());
    std::map<int, uint64_t> keys = node.keys();
    int keys_moved = 0;
    for (auto it = keys.rbegin(); keys_moved != _order / 2; ++it, ++keys_moved) {
        node.removeKey(it->first);
        new_node.addChild(it->first, it->second);
        new_node.setKeysNum(keys_moved + 1);
    }
    _vfs.saveNode(node);
    _vfs.saveNode(new_node);
    return new_node;
}

void BTree::merge(BTreeNode &left, const BTreeNode &right) {
    for (auto key: right.keys()) {
        left.addChild(key.first, key.second);
        left.setKeysNum(left.keysNum() + 1);
    }
}

bool BTree::contains(int key) const {
    BTreeNode root = _vfs.openNode(_root_ref); 
    return contains(root, key);
}

bool BTree::contains(const BTreeNode &node, int key) const {
    if (node.isLeaf())
        return node.contains(key);

    return contains(_vfs.openNode(node.next(key)), key);
}

void BTree::remove(int key) {
    BTreeNode root = _vfs.openNode(_root_ref);
    remove(root, key);
    _vfs.setTreeSize(--_size);

    //root is empty
    if (root.keysNum() == 0 && _size != 0) {
        _root_ref = root.sentinel();
        _vfs.setRootRef(_root_ref);
        _vfs.setTreeSize(--_height);
    }
}

void BTree::remove(BTreeNode &node, int key) {
    if (node.isLeaf()) {
        //remove key from leaf
        node.removeKey(key);        
        _vfs.saveNode(node);
    }
    else {
        BTreeNode next = _vfs.openNode(node.next(key));
        remove(next, key);
        //change key for next if we are removing min element
        if (node.contains(key)) {
            node.removeKey(key);
            node.put(next);
            _vfs.saveNode(node);
        }
        //now we need to balance children
        if (next.keysNum() < _order / 2) {
            if (next.ref() == node.sentinel()) {
                //balancing sentinel
                balanceSentinel(node);
                
            }
            else if (next.minKey() == node.maxKey()) {
                //balance most right node
                balanceWithLeftNode(node, next);
            }
            else {
                balanceWithRightNode(node, next);
                //right neighbour exists
            }
        }
    }
}

void BTree::balanceSentinel(BTreeNode &node) {
    BTreeNode sent = _vfs.openNode(node.sentinel());
    BTreeNode merge_node = _vfs.openNode(node.keys().begin()->second);
    merge(sent, merge_node);
    if (sent.isFull()) {
        BTreeNode new_node = split(sent);
        //node.setSentinel(new_sent.ref());
        node.removeKey(merge_node.minKey());
        node.put(new_node);
        _vfs.saveNode(new_node);
    }
    else {
        node.removeKey(merge_node.minKey());
    }
    _vfs.saveNode(node);
    _vfs.saveNode(sent);
}

void BTree::balanceWithLeftNode(BTreeNode &node, BTreeNode &next) {
    if (next.minKey() == node.minKey()) {
        //node on the left is sentinel
        balanceSentinel(node);
    }
    else {        
        //BTreeNode left_node = _vfs.openNode((--node.keys().find(next.minKey()))->second);
        BTreeNode left_node = _vfs.openNode(node.prevChild(next.minKey()));
        merge(left_node, next);
        if (left_node.isFull()) {
            BTreeNode new_node = split(left_node);
            node.removeKey(next.minKey());
            node.put(new_node);
            _vfs.saveNode(new_node);
        }
        else {
            node.removeKey(next.minKey());
        }
        _vfs.saveNode(node);
        _vfs.saveNode(left_node);
    }

}

void BTree::balanceWithRightNode(BTreeNode &node, BTreeNode &next) {
    //BTreeNode right_node = _vfs.openNode((++node.keys().find(next.minKey()))->second);
    BTreeNode right_node = _vfs.openNode(node.nextChild(next.minKey()));
    merge(next, right_node);
    if (next.isFull()) {
        BTreeNode new_node = split(next);
        node.removeKey(right_node.minKey());
        node.put(new_node);
        _vfs.saveNode(new_node);
    }
    else {
        node.removeKey(right_node.minKey());
    }
    _vfs.saveNode(node);
    _vfs.saveNode(next);
}

uint64_t BTree::size() const {
    return _size;
}

int BTree::height() const {
    return _height;
}

BTree::iterator BTree::begin() const {
    if (_size == 0) return iterator(this, 0, true);
    return iterator(this, minKey(), false);
}

BTree::iterator BTree::end() const {
    return iterator(this, 0, true);
}

void BTree::print() const {
    BTreeNode root = _vfs.openNode(_root_ref);
    print(root, 0);
}

void BTree::print(const BTreeNode &node, int level) const {
    std::cout << level << ": ";
    for (auto key: node.keys()) {
       std::cout << key.first << ",";
    }
    std::cout << std::endl;
    if (node.sentinel() != 0) {
        BTreeNode sent = _vfs.openNode(node.sentinel());
        print(sent, level + 1);
    }
    for (auto key: node.keys()) {
        if (key.second != 0) {
            BTreeNode next = _vfs.openNode(key.second);
            print(next, level + 1);
        }
    }
}

int BTree::findNext(int key) const {
    BTreeNode root = _vfs.openNode(_root_ref);
    return findNext(root, key);
}

int BTree::findNext(const BTreeNode &node, int key) const {
    if (node.isLeaf()) {
        if (!node.contains(key) || key == node.maxKey()) {
            return key;
        }
        auto it = node.keys().find(key);
        return (++it)->first;
    }
    else {
        BTreeNode next = _vfs.openNode(node.next(key));
        int found = findNext(next, key);
        if (found != key) return found;

        if (next.ref() == node.sentinel())
            return node.minKey();

        auto it = node.keys().find(next.minKey());
        if (it->first == node.maxKey()) return key;
        return (++it)->first;    
    }
}

int BTree::minKey() const {
    if (_size == 0) {
        throw std::logic_error("BTree is empty");
    }
    BTreeNode root = _vfs.openNode(_root_ref);
    return minKey(root);
}

int BTree::minKey(const BTreeNode &node) const {
    if (node.isLeaf())
        return node.minKey();
    if (node.sentinel() != 0)
        return minKey(_vfs.openNode(node.sentinel()));
    uint64_t next = node.keys().begin()->second;
    return minKey(_vfs.openNode(next));
}

bool BTree::checkValid() const {
    if (_root_ref == 0)
        return _size == 0;
    BTreeNode root = _vfs.openNode(_root_ref);
    return checkValid(root, _height);
}

bool BTree::checkValid(const BTreeNode &node, int height) const {
    if (_root_ref != node.ref() && node.keysNum() < _order / 2) return false;
    if (node.isFull()) return false;
    if (node.isLeaf()) {
        if (height != 0) return false;
    }
    else {
        if (node.sentinel() != 0) {
            BTreeNode sent = _vfs.openNode(node.sentinel());
            if (!checkValid(sent, height - 1)) return false;
            if (sent.maxKey() >= node.minKey()) return false;
        }
        std::map<int, uint64_t> keys = node.keys();
        for (auto it = keys.begin(); it != keys.end(); ++it) {
            BTreeNode child = _vfs.openNode(it->second);
            if (!checkValid(child, height - 1)) return false;
            if (it->first != child.minKey()) return false;
            auto it1 = it;
            if (++it1 != keys.end()) {
                if (child.maxKey() >= it1->first) return false;
            }
        }
    }
    return true;
}

BTree::~BTree() { }

BTree::iterator::iterator(const BTree *tree, int key, bool is_end):
    _tree(tree),
    _key(key),
    _is_end(is_end) { }

BTree::iterator & BTree::iterator::operator++() {
    if (_is_end) {
        throw std::logic_error("Invalid iterator operation: increment end() iterator");
    }
    int found = _tree->findNext(_key);
    if (found == _key) 
        _is_end = true;
    else 
        _key = found;
    return *this;
}

bool operator==(const BTree::iterator &a, const BTree::iterator &b) {
    if (a._is_end == b._is_end) return true;
    return a._key == b._key;
}

bool operator!=(const BTree::iterator &a, const BTree::iterator &b) {
    return !(a == b);
}

int BTree::iterator::operator*() const {
    if (_is_end) {
        throw std::logic_error("Invalid iterator operation: dereferencing end() iterator");
    }
    return _key;
}

