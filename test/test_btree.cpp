#include "../btree.h"
#include <iostream>
#include <assert.h>

void test_invalid_put(BTree &tree) {
    try {
        tree.put(46);
    }
    catch (const std::logic_error &e) {
        return;
    }
    assert(false);
}

void test_contains(const BTree &tree) {
    assert(tree.contains(21));
    assert(tree.contains(93));
    assert(!tree.contains(0));
    assert(!tree.contains(75));
}

int main() {
    BTree tree("test_btree.dat", 4);
    tree.put(1);
    tree.put(23);
    tree.put(76);
    tree.put(235);
    tree.put(21);
    tree.put(46);
    tree.put(28);
    tree.put(11);
    tree.put(93);
    tree.put(-34);
    tree.put(-354);
    tree.put(52);
    assert(tree.size() == 12);
    test_invalid_put(tree);
    test_contains(tree);
}
