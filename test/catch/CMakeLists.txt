set(LIBRARY_NAME ${PROJECT_NAME}-catch-main)

set(SRCS    src/catch.cpp
            src/logger.cpp
   )

set(DEPENDENCIES
        catch
        log
        #core
    )

configure_file(include/unittest/config.h.in include/unittest/config.h)

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/unittest)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(unittest ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
