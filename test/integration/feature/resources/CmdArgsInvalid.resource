*** Keywords ***
we call the application with an invalid command line
    [Arguments]     ${args}
    run_application     ${args}
    print_stdout
    print_stderr
