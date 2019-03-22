*** Keywords ***
the usage information should be printed to stdout
    stdout contains     Usage:
    stdout contains     Optional arguments:

we call the application with the help option and other command line options
    [Arguments]     ${args}
    we call the application with command line options   ${args}
