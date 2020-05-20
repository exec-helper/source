set(MODULE_NAME plugins)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
        src/luaPlugin.cpp
        src/pmd.cpp
        src/pluginUtils.cpp
        src/memory.cpp
        src/commandPlugin.cpp
        src/valgrind.cpp
        src/executePlugin.cpp
        src/plugin.cpp
        src/commandLineCommand.cpp
        src/lcov.cpp
        src/logger.cpp
        src/commandLine.cpp
        src/verbosity.cpp
        src/addEnvironment.cpp
        src/threadedness.cpp
   )

set(DEPENDENCIES
        log
        config
        core
        lua-wrapper
   )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/plugins)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(PLUGINS
    src/scripts/make.lua
    src/scripts/cmake.lua
    src/scripts/docker.lua
    src/scripts/ninja.lua
    src/scripts/bootstrap.lua
    src/scripts/scons.lua
    src/scripts/clang-tidy.lua
    src/scripts/cppcheck.lua
    src/scripts/selector.lua
    src/scripts/clang-static-analyzer.lua
)
install(FILES ${PLUGINS} DESTINATION  share/exec-helper/plugins COMPONENT runtime)
