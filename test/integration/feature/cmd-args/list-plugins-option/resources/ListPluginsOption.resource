*** Settings ***
Resource     ../../../resources/Common.resource

*** Keywords ***
the plugins information should be printed to stdout
    stdout contains     plugins
    stdout contains     command-line-command

we call the application with the list plugins option and other command line options which are not the help option
    [Arguments]     ${args}
    we call the application with command line options   ${args}
