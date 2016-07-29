#include "../btree.h"

#include <stdexcept>
#include <assert.h>
#include <iostream>

int main() {
    BTree tree("test_btree_100000.dat", 100);
    for (int i = 0; i < 100000; i++) {
        tree.put(i);
    }
    assert(tree.height() == 2);
    assert(tree.size() == 100000);
    
    BTree tree_rand("test_btree_100000_rand.dat", 50);
    for (int i = 0; i < 100000; i++) {
        try {
            tree_rand.put(rand());
        }
        catch (const std::logic_error &e) {
            //key exists            
            std::cout << e.what() << std::endl;
        }
    }
    assert(tree_rand.checkValid());
}
