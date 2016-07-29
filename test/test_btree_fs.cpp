#include "../btree_fs.h"
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include <iostream>



void test_save_open_node() {
    BTreeFS fs("test_btree_fs.dat", 4);
    BTreeNode node = fs.allocNode(true);
    node.setSentinel(rand());
    node.setKeysNum(4);
    node.addChild(23, rand());
    node.addChild(48, rand());
    node.addChild(236, rand());
    node.addChild(326, rand());
    fs.saveNode(node);
    BTreeNode saved_node = fs.openNode(node.ref());
    assert(node == saved_node);
}

int main() {
    test_save_open_node();
}
