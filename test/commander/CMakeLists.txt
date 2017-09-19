set(EXE_NAME ${PROJECT_NAME}-commander-unittest)

set(SRCS
        src/commanderTest.cpp
   )

set(DEPENDENCIES
        commander
        test-utils
        unittest
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
