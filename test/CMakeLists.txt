cmake_minimum_required(VERSION 2.8)

set (TESTS test_btree_fs
    test_btree_node
    test_btree
    test_btree_create
    test_btree_search
    test_btree_it
    test_btree_remove
)
foreach(testname ${TESTS})
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} btree)
    add_test(${testname} ${testname})
endforeach(testname)


