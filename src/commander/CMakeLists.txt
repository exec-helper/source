set(MODULE_NAME commander)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
    src/commander.cpp
   )

set(DEPENDENCIES
        log
        core
        plugins
    )

include_directories(
        include/commander
    )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/commander)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
