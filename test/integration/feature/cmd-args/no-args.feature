@cmd_args @no_args
Feature: Calling exec-helper without command-line options
    Scenarios for calling exec-helper without command-line options

    Background:
        Given a controlled environment

    @successful
    Scenario: The application is called with no command line arguments and no valid configuration file
        When we call the application without arguments
        Then the call should fail with return code 1
        And stderr should contain 'Could not find a settings file'

    @successful
    Scenario: The application is called with no command line arguments and a valid configuration file
        Given a valid configuration
        When we call the application without arguments
        Then the call should fail with return code 1
        And stderr should contain 'must define at least one command'
