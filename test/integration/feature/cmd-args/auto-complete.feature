@cmd_args @auto_complete_option
Feature: Use the auto-complete command-line option
    Scenarios for when the auto-complete option is given on the command line

    Examples:    
    | command_line                                    |
    | --auto-complete some                            |
    | --auto-complete some --dry-run --debug debug    |
    | --debug debug --auto-complete some --dry-run    |
    | --dry-run --debug debug --auto-complete some    |

    Background:
        Given a controlled environment

    @successful
    Scenario: The auto-complete option is defined without a configuration file
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain regex '^-h$'
        And stdout should contain regex '^--help$'
        And stdout should contain regex '^--version$'
        And stdout should contain regex '^-v$'
        And stdout should contain regex '^--verbose$'
        And stdout should contain regex '^-j$'
        And stdout should contain regex '^--jobs$'
        And stdout should contain regex '^-n$'
        And stdout should contain regex '^--dry-run$'
        And stdout should contain regex '^--list-plugins$'
        And stdout should contain regex '^--additional-search-path$'
        And stdout should contain regex '^-s$'
        And stdout should contain regex '^--settings-file$'
        And stdout should contain regex '^-d$'
        And stdout should contain regex '^--debug$'
        And stdout should contain regex '^--auto-complete$'
        And stdout should contain regex '^-z$'
        And stdout should contain regex '^--command$'

    @successful
    Scenario: The auto-complete option is defined on a valid command line
        Given a valid configuration
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain regex '^-h$'
        And stdout should contain regex '^--help$'
        And stdout should contain regex '^--version$'
        And stdout should contain regex '^-v$'
        And stdout should contain regex '^--verbose$'
        And stdout should contain regex '^-j$'
        And stdout should contain regex '^--jobs$'
        And stdout should contain regex '^-n$'
        And stdout should contain regex '^--dry-run$'
        And stdout should contain regex '^--list-plugins$'
        And stdout should contain regex '^--additional-search-path$'
        And stdout should contain regex '^-s$'
        And stdout should contain regex '^--settings-file$'
        And stdout should contain regex '^-d$'
        And stdout should contain regex '^--debug$'
        And stdout should contain regex '^--auto-complete$'
        And stdout should contain regex '^-z$'
        And stdout should contain regex '^--command$'

    @successful
    Scenario: The auto-complete option is defined with a command
        Given a valid configuration
        And the <command> command
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain the full line <command>

        Examples:
        | command  |
        | Command1 |

    @successful
    Scenario: The auto-complete option is defined with a pattern
        Given a valid configuration
        And the <pattern> pattern
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain the full line <pattern>

        Examples:
        | pattern                                                                      |
        | { "key": "PATTERN", "long_options": ["blaat"], "default_values": ["blaat"] } |

    @successful
    Scenario: The auto-complete option is defined with a pattern and a command
        Given a valid configuration
        And the <command> command
        And the <pattern> pattern
        When we add the <command_line> as command line arguments
        And we call the application
        Then the call should succeed
        And stdout should contain the full line <pattern>

        Examples:
        | command  | pattern                                                                      |
        | Command1 | { "key": "PATTERN", "long_options": ["blaat"], "default_values": ["blaat"] } |
