set(MODULE config)

set(ROBOT_FILES
    __init__.robot
    )

install(FILES ${ROBOT_FILES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE} COMPONENT integration)

add_subdirectory(environment)
