function(GenerateSimpleRobotTest TEST_NAME SUB_DIR)
    file(READ ${TEST_NAME}.resources RESOURCES)
    file(READ ${TEST_NAME}.scenarios SCENARIOS)

    configure_file("${PROJECT_SOURCE_DIR}/test/integration/feature/simple-test.robot.template" "${TEST_NAME}.robot" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot" DESTINATION "${INTEGRATION_TEST_BIN_DIR}/${SUB_DIR}" COMPONENT integration)
endfunction()

function(GenerateDataDrivenRobotTest TEST_NAME SUB_DIR)
    file(READ ${TEST_NAME}.data DATA)
    file(READ ${TEST_NAME}.resources RESOURCES)
    file(READ ${TEST_NAME}.scenarios ALL_SCENARIOS)

    set(FOUND_SCENARIO "")
    set(SCENARIOS)

    string(FIND ${ALL_SCENARIOS} "File:" FOUND_POSITION REVERSE)
    while(NOT FOUND_POSITION EQUAL -1)
        string(SUBSTRING ${ALL_SCENARIOS} ${FOUND_POSITION} -1 FOUND_SCENARIO) 
        string(SUBSTRING ${ALL_SCENARIOS} 0 ${FOUND_POSITION} ALL_SCENARIOS) 

        list(APPEND SCENARIOS ${FOUND_SCENARIO})

        if(ALL_SCENARIOS)
            string(FIND ${ALL_SCENARIOS} "File:" FOUND_POSITION REVERSE)
        else()
            set(FOUND_POSITION -1)
        endif()
    endwhile()

    foreach(SCENARIO IN LISTS SCENARIOS)
        # Find filename
        string(REGEX MATCH "File: [A-Za-z0-9-]*" TMP ${SCENARIO})
        string(SUBSTRING ${TMP} 6 -1 SCENARIO_FILE)

        # Remove filename from scenario
        string(LENGTH "${TMP}" FILENAME_LENGTH)
        string(SUBSTRING ${SCENARIO} ${FILENAME_LENGTH} -1 SCENARIO)

        # Get scenario name
        string(REGEX MATCH "Scenario:[A-Za-z0-9 ]*" SCENARIO_NAME ${SCENARIO})

        configure_file("${PROJECT_SOURCE_DIR}/test/integration/feature/data-driven-test.robot.template" "${SCENARIO_FILE}.robot" @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${SCENARIO_FILE}.robot" DESTINATION "${INTEGRATION_TEST_BIN_DIR}/${SUB_DIR}" COMPONENT integration)
    endforeach()
endfunction()

set(RESOURCES
    resources/Common.resource
    resources/Config.py
    resources/TestEnvironment.py
    resources/WorkingDir.py
    resources/command.py
)
install(FILES ${RESOURCES} DESTINATION ${INTEGRATION_TEST_BIN_DIR}/resources COMPONENT integration)

add_subdirectory(cmd-args)
add_subdirectory(config)
