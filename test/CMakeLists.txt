set(UNITTEST_BIN_DIR    ${BIN_DIRECTORY}/test/unittest)

add_subdirectory(catch)
add_dependencies(${PROJECT_NAME}-catch-main ${${PROJECT_NAME}-log_DEPENDENCIES})    # Since log does not create any files itself, depend on a another target that is low enough in the dependency tree

add_subdirectory(utils)
add_dependencies(${PROJECT_NAME}-test-utils ${${PROJECT_NAME}-log_DEPENDENCIES})    # Since log does not create any files itself, depend on a another target that is low enough in the dependency tree

add_subdirectory(config)
add_subdirectory(yaml)
add_subdirectory(core)
add_subdirectory(plugins)
add_subdirectory(commander)
