set(MODULE "${MODULE}/invalid-args")

set(DATA_DRIVEN_TESTS
        invalid-args
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    GenerateDataDrivenRobotTest(${FEATURE} "${MODULE}")
endforeach()

set(RESOURCES
    resources/InvalidArgs.resource
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE}/resources COMPONENT integration)
