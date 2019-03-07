*** Settings ***
Library     TestEnvironment.py

*** Keywords ***
a test environment with a valid config
    add_valid_config

A test environment with no config
    no_config

we call the application with command line options
    [Arguments]     ${args}
    run_application     ${args}
    print_stdout
    print_stderr

return code should be
    [Arguments]     ${expected_ret_code}
    check_return_code    ${expected_ret_code}

the call should succeed
    return code should be   0
