cmake_minimum_required(VERSION 3.1)

project(exec-helper)

set (CMAKE_CXX_STANDARD 11)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_definitions(-Wall -Wextra -Werror)

    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER )
    if(${BUILD_TYPE_LOWER} MATCHES debug)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILE_FLAGS}" )
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINK_FLAGS}" )
    endif(${BUILD_TYPE_LOWER} MATCHES debug)
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

option(LIMITED_OPTIMIZATION "Use only limited optimization. This is mostly interesting for certain analysis tools." OFF)

if(LIMITED_OPTIMIZATION)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_definitions(-O1)
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
endif(LIMITED_OPTIMIZATION)

set(BIN_DIRECTORY bin)
set(LIB_DIRECTORY lib)

add_subdirectory(3rdparty)
add_subdirectory(src)

add_subdirectory(test)
add_subdirectory(docs)
add_subdirectory(gitchangelog)
