@config @environment
Feature: Test settings the environment for the configured commands
    Scenarios for setting the environment for configured command(s)

    Background:
        Given a controlled environment
        And a valid configuration

    @successful
    Scenario: Set the environment to a fixed value
        Given the <command> command
        And the <environment> is configured for <command> command in the configuration
        When we run the <command> command
        Then the call should succeed
        And the runtime environment for <command> should contain the given <environment>

        Examples:    
        | command   | environment                           |
        | Command1  | KEY1:VALUE1                           |
        | Command2  | KEY1:VALUE1;KEY2:VALUE2;KEY3:VALUE3   |
    
    @successful
    Scenario: Replace patterns in the configured environment
        Given the <command> command
        And the <pattern> pattern
        And the <pattern> is configured for <command> command in the configuration
        And the <environment> is configured for <command> command in the configuration
        When we run the <command> command
        Then the call should succeed
        And the runtime environment for <command> should contain the given <environment>

        Examples:
        | command  | pattern                                             | environment                               |
        | Command1 | { "key": "PATTERN", "default_values": ["blaat"] }   | KEY:{PATTERN}                             |
        | Command1 | { "key": "PATTERN", "default_values": ["blaat"] }   | {PATTERN}:VALUE                           |
        | Command1 | { "key": "PATTERN", "default_values": ["blaat"] }   | {PATTERN}:{PATTERN}                       |
        | Command1 | { "key": "PATTERN", "default_values": ["blaat"] }   | this-{PATTERN}-key:this-{PATTERN}-value   |
        | Command1 | { "key": "SPA CE", "default_values": ["bla a at"] } | {SPA CE}:{SPA CE}                         |
