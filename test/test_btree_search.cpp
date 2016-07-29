#include "../btree.h"

#include <stdexcept>
#include <assert.h>
#include <iostream>

int main() {
    BTree tree("test_btree_100000.dat");
    assert(tree.height() == 2);
    assert(tree.size() == 100000);
    for (int i = 0; i < 10; i++) {
        assert(tree.contains(rand() % 100000));
    }
}
