set(MODULE_NAME test-utils)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
        src/utils.cpp
        src/combinationHelpers.cpp
        src/tmpFile.cpp
        src/executionContent.cpp
        src/configFileWriter.cpp
   )

set(DEPENDENCIES
        boost-program-options
        boost-filesystem
        log
        core
        plugins
    )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/utils)
target_include_directories(${LIBRARY_NAME} PUBLIC ../stubs include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
