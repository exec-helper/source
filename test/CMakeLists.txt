set(UNITTEST_BIN_DIR    ${BIN_DIRECTORY})

add_subdirectory(catch)
add_subdirectory(base-utils)
add_subdirectory(utils)
add_subdirectory(log)
add_subdirectory(config)
add_subdirectory(yaml)
add_subdirectory(core)
add_subdirectory(plugins)
add_subdirectory(commander)

set(INTEGRATION_TEST_BIN_DIR ${BIN_DIRECTORY})
add_subdirectory(integration)
