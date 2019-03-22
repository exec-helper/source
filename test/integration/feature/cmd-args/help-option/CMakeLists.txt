set(MODULE "${MODULE}/help-option")

set(DATA_DRIVEN_TESTS
        help-option
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    GenerateDataDrivenRobotTest(${FEATURE} "${MODULE}")
endforeach()

set(RESOURCES
    resources/HelpOption.resource
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE}/resources COMPONENT integration)
