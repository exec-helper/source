@working_dir @settings_file_location
Feature: All paths in a configuration file are relative to the location of the settings file
    Scenarios for checking all paths relative to the settings file

    Examples:
    | command  |
    | Command1 |

    Background:
        Given a controlled environment
        And a valid configuration
        And the <command> command

    @successful
    Scenario: The default working directory is the location of the settings file
        Given a current working directory of <start_working_dir>
        When we run the <command> command
        Then the call should succeed
        And the working directory should be the environment root dir
        And the PWD environment variable should be the environment root dir

        Examples:
        | start_working_dir |
        | /tmp              |
        | .                 |
        | ./blaat           |
        | ./a/b/c/d         |
        | ~                 |
        | /tmp/blaat/       |
