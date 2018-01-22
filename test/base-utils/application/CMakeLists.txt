set(EXE_NAME execution-content)

set(SRCS
        execution-content.cpp
   )

set(DEPENDENCIES
        test-base-utils
        boost-serialization
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

install(TARGETS ${EXE_NAME} DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)
