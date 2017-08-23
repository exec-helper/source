set(MODULE_NAME core)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
        src/pattern.cpp
        src/optionDescriptions.cpp
        src/task.cpp
        src/immediateExecutor.cpp
        src/reportingExecutor.cpp
        src/patterns.cpp
        src/patternsHandler.cpp
        src/execHelperOptions.cpp
        src/posixShell.cpp
        src/argv.cpp
        src/envp.cpp
        src/logger.cpp
   )

set(DEPENDENCIES
        boost-filesystem
        boost-program-options
        log
        config
        yaml
   )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/core)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
