cmake_minimum_required(VERSION 3.0.0)
project(shallow-state-blockchain VERSION 0.1.0)

find_package (Threads)
find_package(Boost REQUIRED COMPONENTS system)

add_executable(node main.cpp node.cpp chain.cpp chain.h transaction.h block.h)
add_executable(client client.cpp)
target_link_libraries (node ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
