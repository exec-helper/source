set(MODULE "${MODULE}/environment")

set(DATA_DRIVEN_TESTS
        fixed-value
        replace-patterns
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    GenerateDataDrivenRobotTest(${FEATURE} "${MODULE}")
endforeach()

set(RESOURCES
    resources/environment.resource
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE}/resources COMPONENT integration)
