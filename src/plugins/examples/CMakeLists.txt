cmake_minimum_required(VERSION 3.0)
project(cmake-example CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(hello src/hello.cpp)
add_executable(world src/world.cpp)
install(TARGETS hello world DESTINATION bin COMPONENT runtime)
