install(DIRECTORY resources DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)

set(FEATURES
        cmd-args-empty
    )

foreach(FEATURE IN LISTS FEATURES)
    file(READ ${FEATURE}.resources RESOURCES)
    file(READ ${FEATURE}.scenarios SCENARIOS)

    configure_file(simple-test.robot.template "${FEATURE}.robot" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${FEATURE}.robot" DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)
endforeach()

set(DATA_DRIVEN_TESTS
        cmd-args-invalid
        help-option
        version-option
    )

foreach(FEATURE IN LISTS DATA_DRIVEN_TESTS)
    file(READ ${FEATURE}.data DATA)
    file(READ ${FEATURE}.resources RESOURCES)
    file(READ ${FEATURE}.scenarios ALL_SCENARIOS)

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

        configure_file(data-driven-test.robot.template "${SCENARIO_FILE}.robot" @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${SCENARIO_FILE}.robot" DESTINATION ${INTEGRATION_TEST_BIN_DIR} COMPONENT integration)
    endforeach()
endforeach()
