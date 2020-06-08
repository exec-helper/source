@cmd_args @dry_run_option
Feature: Use the dry run command-line option
    Scenarios for when the dry run option is given on the command line

    Examples:    
    | command_line                      |
    | -n                                |
    | --dry-run                         |
    | --dry-run --debug debug --verbose |
    | --debug debug --dry-run --verbose |
    | --verbose --debug debug --dry-run |

    Background:
        Given a controlled environment

    @successful
    Scenario: The keep-going option is defined on a valid command line
        Given a valid configuration
        When we add the <command> command
        And we add the <command_line> as command line arguments
        And we add the <command> to the command line options
        When we call the application
        Then the call should succeed
        And the <command> command should be called 0 times

        Examples:
        | command  |
        | describe |
