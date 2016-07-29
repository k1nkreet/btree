#include "../btree.h"

#include <assert.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

int main() {
    BTree tree("test_btree_remove.dat", 10);
    std::vector<int> values;
    for (int i = 0; i < 100; ++i) {
        int val = rand();
        while (std::find(values.begin(), values.end(), val) != values.end())
            val = rand();
        tree.put(val);
        values.push_back(val);
    }
    for (int i = 0; i < 100; ++i) {
        int idx = rand() % values.size();
        tree.remove(values[idx]);
        values.erase(values.begin() + idx);
        assert(tree.checkValid());
        assert(tree.size() == (uint)(99 - i));
    }

}
