cmake_minimum_required(VERSION 3.1)

project(exec-helper)

set (CMAKE_CXX_STANDARD 11)

set(BIN_DIRECTORY bin)
set(LIB_DIRECTORY lib)

add_subdirectory(3rdparty)
add_subdirectory(src)
