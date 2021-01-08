@cmd_args @list_plugins_option
Feature: Use the 'list plugins' command-line option
    Scenarios for when the 'list plugins' option is given on the command line

    Examples:
    | command_line                           |
    | --list-plugins                         |
    | --list-plugins --debug debug --dry-run |
    | --dry-run --list-plugins --debug debug |
    | --debug debug --dry-run --list-plugins |

    Background:
        Given a controlled environment

    @successful
    Scenario: The 'list plugins' option is defined on a valid command line
        Given a valid configuration
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'command-line-command'

    @successful
    Scenario: The 'list plugins' option is defined on a valid command line with no configuration file
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'command-line-command'
