cmake_minimum_required(VERSION 3.10)
project(lab5 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab5 main.c simplefs.h simplefs.c format.c cd.c mkdir.c utils.c close.c ls.c open.c create.c rm.c write.c read.c)