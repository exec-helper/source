cmake_minimum_required(VERSION 3.1)

project(exec-helper
        LANGUAGES CXX)

include(GNUInstallDirs)

# Configure pkg-config (needs to be set explicitly to support cross compilation)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/${CMAKE_INSTALL_LIBDIR}/pkgconfig:${CMAKE_SYSROOT}/usr/${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_FIND_ROOT_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(POSITION_INDEPENDENT_CODE "Create position independent code" ON)
mark_as_advanced(POSITION_INDEPENDENT_CODE)
set(CMAKE_POSITION_INDEPENDENT_CODE ${POSITION_INDEPENDENT_CODE})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

option(ENABLE_WERROR "Enable threating warnings as errors" ON)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    if(${ENABLE_WERROR})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER )
    if(${BUILD_TYPE_LOWER} MATCHES debug)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILE_FLAGS}" )
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINK_FLAGS}" )
    endif(${BUILD_TYPE_LOWER} MATCHES debug)
endif()

# Enable modules for clang
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules")
endif()

option(LIMITED_OPTIMIZATION "Use only limited optimization. This is mostly interesting for certain analysis tools." OFF)
if(LIMITED_OPTIMIZATION)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_definitions(-O1)
    endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
endif(LIMITED_OPTIMIZATION)

option(TERMINATE_ON_ASSERT_FAILURE "Terminate on assertion failure" OFF)
MESSAGE(STATUS "Terminate on assert failure: " ${TERMINATE_ON_ASSERT_FAILURE})
if(${TERMINATE_ON_ASSERT_FAILURE})
    add_definitions(-DTERMINATE_ON_ASSERT_FAILURE)
endif()

set(BIN_DIRECTORY ${CMAKE_INSTALL_BINDIR})
set(LIB_DIRECTORY ${CMAKE_INSTALL_LIBDIR})

option(ENABLE_TESTING "Enable building tests" ON)

add_subdirectory(3rdparty)
add_subdirectory(src)

if(${ENABLE_TESTING})
    enable_testing()
    add_subdirectory(test)
endif()
add_subdirectory(docs)
add_subdirectory(gitchangelog)
