cmake_minimum_required(VERSION 3.0.0)
project(shallow-state-blockchain VERSION 0.1.0)

find_package (Threads)
find_package(Boost REQUIRED COMPONENTS system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} cmake/)
include_directories(${CMAKE_SOURCE_DIR})
include_directories("./ate-pairing/include/")
include_directories("./ate-pairing/test/")
link_directories("./ate-pairing/lib/")

add_executable(node main.cpp workload.cpp workload.h chain.cpp chain.h transaction.h block.h VC.cpp VC.h edrax/vcs.cpp config.cpp config.h LRU/LRU.h)
add_executable(order-lru LRU/order-tree/main.cpp LRU/order-tree/lru.hpp LRU/order-tree/order_tree.hpp)
add_executable(lru-test LRU/test.cpp)
#add_executable(client client.cpp transaction.h api.h)
target_link_libraries (node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} gmp gmpxx zm)
#target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
