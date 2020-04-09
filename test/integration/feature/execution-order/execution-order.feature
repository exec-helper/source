@execution_order
Feature: Execution order
    The order of execution must be as defined by the exec-helper configuration and specification

    Background:
        Given a controlled environment
        And a valid configuration

    @successful
    Scenario: Run a command with one associated command line a number of times
        When we add the <command> command
        And run the <command> command <nb_of_times> in the same statement
        Then the call should succeed
        And the <command> command should be called <nb_of_times> times
        And stderr should be empty

        Examples:    
        | command       | nb_of_times   |
        | some-command  |   1           |
        | other-command |   10          |
