@cmd_args @help_option
Feature: Use the help command-line option
    Scenarios for when the help option is given on the command line

    Examples:    
    | command_line                      |
    | -h                                |
    | --help                            |
    | --help --version --debug debug    |
    | --debug debug --help --version    |
    | --version --debug debug --help    |

    Background:
        Given a controlled environment

    @successful
    Scenario: The help option is defined on a valid command line
        Given a valid configuration
        When we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain 'Usage'
        And stdout should contain '--help'
        And stdout should contain '-h'
        And stdout should contain '--version'

    @successful
    Scenario: The help option is defined on a valid command line with no configuration file
        When we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain 'Usage'
        And stdout should contain '--help'
        And stdout should contain '-h'
        And stdout should contain '--version'
