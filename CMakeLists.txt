cmake_minimum_required(VERSION 2.8)
project(btree)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

set(SOURCES btree_fs.cpp btree_node.cpp btree.cpp)
set(HEADERS btree_fs.h btree_node.h btree.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(test)
