set(MODULE "${MODULE}/list-plugins-option")

set(DATA_DRIVEN_TESTS
        list-plugins-option
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    GenerateDataDrivenRobotTest(${FEATURE} "${MODULE}")
endforeach()

set(RESOURCES
    resources/ListPluginsOption.resource
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE}/resources COMPONENT integration)
