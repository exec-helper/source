set(MODULE_NAME test-utils)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

find_package(Boost REQUIRED COMPONENTS program_options filesystem)

set(SRCS
        src/utils.cpp
        src/combinationHelpers.cpp
        src/tmpFile.cpp
   )

set(DEPENDENCIES
        log
        core
        plugins
    )

set(LIBRARY_INCLUDES
        ${Boost_INCLUDE_DIR}
    )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/utils)
target_include_directories(${LIBRARY_NAME} PUBLIC ../stubs include ${DEPENDENCIES} ${LIBRARY_INCLUDES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
