set(MODULE_NAME plugins)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
        src/bootstrap.cpp
        src/pmd.cpp
        src/selector.cpp
        src/make.cpp
        src/pluginUtils.cpp
        src/memory.cpp
        src/commandPlugin.cpp
        src/cppcheck.cpp
        src/valgrind.cpp
        src/executePlugin.cpp
        src/plugin.cpp
        src/scons.cpp
        src/commandLineCommand.cpp
        src/buildPlugin.cpp
        src/clangStaticAnalyzer.cpp
        src/clangTidy.cpp
        src/lcov.cpp
        src/logger.cpp
   )

set(DEPENDENCIES
        boost-optional
        log
        config
        core
   )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/plugins)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
