set(EXE_NAME ${PROJECT_NAME}-integration-test)

set(SRCS    src/executeMultipleStatements.cpp
   )

set(DEPENDENCIES
        yaml-cpp
        gsl
        log
        test-base-utils
        unittest
   )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)
add_custom_target(integration COMMENT "Building integration tests" )
add_dependencies(integration ${EXE_NAME})
