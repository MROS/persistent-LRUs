cmake_minimum_required(VERSION 3.0.0)
project(LRU VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
add_executable(lru_test test.cpp LRU.h simple-copy/lru.hpp simple-copy/mutable-lru.hpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
