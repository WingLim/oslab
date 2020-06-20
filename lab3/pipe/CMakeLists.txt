cmake_minimum_required(VERSION 3.10)
project(pipe C)

set(CMAKE_C_STANDARD 99)

find_package( Threads )

add_executable(pipe main.c)

target_link_libraries( pipe ${CMAKE_THREAD_LIBS_INIT} )