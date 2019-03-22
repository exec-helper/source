set(MODULE "${MODULE}/version-option")

set(DATA_DRIVEN_TESTS
        version-option
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    GenerateDataDrivenRobotTest(${FEATURE} "${MODULE}")
endforeach()

set(RESOURCES
    resources/VersionOption.resource
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/${MODULE}/resources COMPONENT integration)
