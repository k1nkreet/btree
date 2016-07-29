#include "../btree.h"
#include <vector>
#include <assert.h>
#include <iostream>

int main() {
    BTree tree("test_btree_it.dat", 3);
    tree.put(1);
    tree.put(-1);
    tree.put(5);
    tree.put(6);
    tree.put(18);
    tree.put(23);
    tree.put(-8);
    tree.put(-76);
    std::vector<int> check;
    for (BTree::iterator it = tree.begin(); it != tree.end(); ++it) {
        check.push_back(*it);
    }
    assert(check[0] == -76);
    assert(check[1] == -8);
    assert(check[2] == -1);
    assert(check[3] == 1);
    assert(check[4] == 5);
    assert(check[5] == 6);
    assert(check[6] == 18);
    assert(check[7] == 23);
    assert(check.size() == 8);
}
