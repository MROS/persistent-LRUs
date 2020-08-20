cmake_minimum_required(VERSION 3.0.0)
project(LRU VERSION 0.1.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_CXX_STANDARD 17)
add_executable(lru_test test.cpp LRU.h memory.hpp
        simple-copy/lru.hpp simple-copy/mutable-lru.hpp
        rb-tree/lru.hpp rb-tree/node.hpp rb-tree/rb_tree.hpp
        order-tree/lru.hpp order-tree/order_tree.hpp
        heap/lru.hpp heap/heap.hpp
        )
add_executable(immutable_lru_test test_immutable.cpp LRU.h memory.hpp
        simple-copy/lru.hpp simple-copy/mutable-lru.hpp
        rb-tree/lru.hpp rb-tree/node.hpp rb-tree/rb_tree.hpp
        order-tree/lru.hpp order-tree/order_tree.hpp
        heap/lru.hpp heap/heap.hpp
        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
