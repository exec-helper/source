set(MODULE cmd-args)

set(ROBOT_FILES
    __init__.robot
    )

install(FILES ${ROBOT_FILES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE} COMPONENT integration)

add_subdirectory(no-args)
add_subdirectory(invalid-args)
add_subdirectory(help-option)
add_subdirectory(version-option)
add_subdirectory(list-plugins-option)
