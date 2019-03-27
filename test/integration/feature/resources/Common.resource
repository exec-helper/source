*** Settings ***
Library     TestEnvironment.py

*** Keywords ***
a test environment with a valid config
    add_valid_config

a test environment with no config
    no_config

an executable command
    [Arguments]     ${command}
    Create command  ${command}

the environment set in the config
    [Arguments]     ${command}     ${envs}
    Set environment     ${command}      ${envs}

we call the application without command line arguments
    write_config
    run_application
    print_stdout
    print_stderr

we call the application with command line options
    [Arguments]     ${args}
    write_config
    run_application     ${args}
    print_stdout
    print_stderr

When we call the application with the command
    [Arguments]     @{command}
    write_config
    run_application     ${command}
    print_stdout
    print_stderr

the return code should be
    [Arguments]     ${expected_ret_code}
    check_return_code    ${expected_ret_code}

the call should fail with return code
    [Arguments]     ${expected_ret_code}
    the return code should be   ${expected_ret_code}

the call should succeed
    the return code should be   0

stdout must contain
    [Arguments]     ${message}
    stdout_contains     ${message}

stderr must contain
    [Arguments]     ${message}
    stderr_contains     ${message}
