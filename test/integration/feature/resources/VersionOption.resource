*** Settings ***
Resource     Common.resource

*** Keywords ***
the version information should be printed to stdout
    stdout contains     exec-helper
    stdout contains     COPYRIGHT

we call the application with the version option and other command line options which are not the help option
    [Arguments]     ${args}
    we call the application with command line options   ${args}
