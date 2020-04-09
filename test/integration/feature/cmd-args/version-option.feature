@cmd_args @version_option
Feature: Use the help command-line option
    Scenarios for when the help option is given on the command line

    Examples:    
    | command_line                      |
    | --version                         |
    | --version --debug debug --dry-run |
    | --debug debug --version --dry-run |
    | --dry-run --debug debug --version |

    Background:
        Given a controlled environment

    @successful
    Scenario: The version option is defined on a valid command line
        Given a valid configuration
        When we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain 'exec-helper'
        And stdout should contain 'COPYRIGHT'

    @successful
    Scenario: The version option is defined on a valid command line with no configuration file
        When we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain 'exec-helper'
        And stdout should contain 'COPYRIGHT'
