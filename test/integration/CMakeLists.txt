add_custom_target(integration COMMENT "Building integration tests" )

add_subdirectory(feature)

set(EXE_NAME ${PROJECT_NAME}-integration-test)

set(SRCS
            src/executeMultipleStatements.cpp
   )

set(DEPENDENCIES
        yaml-3rd
        gsl
        log
        test-base-utils
        unittest
   )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE include/integration ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})
add_dependencies(${EXE_NAME} execution-content)

install(TARGETS ${EXE_NAME} DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)
add_dependencies(integration ${EXE_NAME})
