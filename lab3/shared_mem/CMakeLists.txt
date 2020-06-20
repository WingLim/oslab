cmake_minimum_required(VERSION 3.10)
project(shared_mem C)

set(CMAKE_C_STANDARD 99)

find_package(Threads)

add_executable(sender sender.c)
add_executable(recv recv.c)

target_link_libraries(sender ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(recv ${CMAKE_THREAD_LIBS_INIT})