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
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'Usage'
        And stdout should contain 'Optional arguments:'
        And stdout should not contain 'Configured commands:'

    @successful
    Scenario: The help option is defined on a valid command line with no configuration file
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'Usage: exec-helper [Optional arguments] COMMANDS...'
        And stdout should contain 'Optional arguments:'
        And stdout should not contain 'Configured commands:'

    @successful
    Scenario: The help option is defined for a configuration with a command
        Given the <command> command
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'Usage: exec-helper [Optional arguments] COMMANDS...'
        And stdout should contain 'Optional arguments:'
        And stdout should contain 'Configured commands:'
        And stdout should contain <command>

        Examples:
        | command  |
        | Command1 |

    @successful
    Scenario: The help option is defined for a configuration with a pattern
        Given the <pattern> pattern
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'Usage: exec-helper [Optional arguments] COMMANDS...'
        And stdout should contain 'Optional arguments:'
        And stdout should not contain 'Configured commands:'
        And stdout should contain 'Values for pattern'

        Examples:
        | pattern                                                                      |
        | { "key": "PATTERN", "long_options": ["blaat"], "default_values": ["blaat"] } |

    @successful
    Scenario: The help option is defined for a configuration with a pattern and a command
        Given the <command> command
        And the <pattern> pattern
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain 'Usage: exec-helper [Optional arguments] COMMANDS...'
        And stdout should contain 'Optional arguments:'
        And stdout should contain 'Configured commands:'
        And stdout should contain 'Values for pattern'
        And stdout should contain <command>

        Examples:
        | command  | pattern                                                                       |
        | Command1 | { "key": "PATTERN", "long_options": ["blaat"], "default_values": ["blaat"] }  |
