@cmd_args @custom_plugins @custom_plugins_usage
Feature: Using custom plugins
    Scenarios for using custom plugins

    Examples:
    | plugin_id                 | command  |
    | exec-helper-custom-module | Command1 |
    | make                      | Command2 |

    Background:
        Given a controlled environment
        And a valid configuration
        And a random custom plugin directory
        And a custom module with id <plugin_id>
        And a registered command <command> that uses the module <plugin_id>
        And the custom plugin search path is registered in the configuration

    @successful
    Scenario: Check that the custom plugin is called
        When run the <command> command <nb_of_times> in the same statement
        Then the call should succeed
        And the <command> command should be called <nb_of_times> times
        And stderr should be empty

        Examples:    
        | nb_of_times   |
        |   1           |
        |   10          |
