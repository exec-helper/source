@cmd_args @invalid_args
Feature: Call the application with invalid arguments
    Scenarios for when the application is called with invalid command-line arguments

    Examples:    
    | command_line              |
    | -b                        |
    | --blaat                   |
    | -b blaat                  |
    | --blaat blaat             |
    | --blaat blaat --foo bar   |

    Background:
        Given a controlled environment

    @error
    Scenario: The version option is defined on a valid command line
        Given a valid configuration
        When we call the application with the <command_line> options
        Then the call should fail with return code 1
        And stderr should contain 'unrecognised option'
        And stdout should contain 'Usage'
        And stdout should contain '--help'

    @error
    Scenario: The version option is defined on a valid command line with no configuration file
        When we call the application with the <command_line> options
        Then the call should fail with return code 1
        And stderr should contain 'unrecognised option'
        And stdout should contain 'Usage'
        And stdout should contain '--help'
