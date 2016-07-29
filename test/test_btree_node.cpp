#include <cstdlib>
#include <assert.h>
#include <iostream>
#include "../btree_node.h"

void test_serialization(BTreeNode &node) {
    uint8_t page[4098];
    int size = node.serialize(page);
    BTreeNode node1 = BTreeNode::deserialize(page, size);
    assert(node == node1);
}

int main() {
    BTreeNode node = BTreeNode(4, rand(), true);
    node.setSentinel(53489);
    node.addChild(10, 23423);
    node.addChild(23, 43512);
    node.addChild(-3, 32321);
    node.addChild(-674, 12352);
    node.setKeysNum(4);
    //test_serialization
    test_serialization(node);
    //test min/max key
    assert(node.minKey() == -674);
    assert(node.maxKey() == 23);
    //test contains
    assert(node.contains(23));
    assert(!node.contains(34));
    assert(node.contains(10));
    assert(node.contains(-674));
    assert(node.contains(-3));
    assert(!node.contains(5));
    assert(!node.contains(38));
    //test next/prev child
    assert(node.nextChild(10) == 43512);
    assert(node.nextChild(23) == 0);
    assert(node.nextChild(-3) == 23423);
    assert(node.nextChild(-674) == 32321);
    assert(node.nextChild(982) == 0);        
    assert(node.prevChild(10) == 32321);
    assert(node.prevChild(23) == 23423);
    assert(node.prevChild(-3) == 12352);
    assert(node.prevChild(-674) == 0);
    assert(node.prevChild(982) == 0);
    //test next
    assert(node.next(-1234) == 53489);
    assert(node.next(10) == 23423);
    assert(node.next(18) == 23423);
    assert(node.next(23) == 43512);
    assert(node.next(78) == 43512);
    assert(node.next(-3) == 32321);
    assert(node.next(0) == 32321);
    assert(node.next(-674) == 12352);
    assert(node.next(-523) == 12352);
    //test put
    BTreeNode new_node = BTreeNode(4, rand(), true);
    new_node.put(-5);
    new_node.put(-1);
    new_node.put(4);
    new_node.put(8);
    node.put(new_node);
    assert(node.contains(-5));
    assert(node.keysNum() == 5);
}
