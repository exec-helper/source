@cmd_args @keep_going_option
Feature: Use the keep-going command-line option
    Scenarios for when the keep-going option is given on the command line

    Examples:    
    | command_line                         |
    | -k                                   |
    | --keep-going                         |
    | --keep-going --debug debug --verbose |
    | --debug debug --keep-going --verbose |
    | --verbose --debug debug --keep-going |

    Background:
        Given a controlled environment

    @successful
    Scenario: The keep-going option is defined on a valid command line
        Given a valid configuration
        When we add the <command> that returns <return_code>
        And we add the <command_line> as command line arguments
        And we add the <command> <nb_of_times> to the command line options
        When we call the application
        Then the call should fail with return code <return_code>
        And the <command> command should be called <nb_of_times> times

        Examples:
        | command | return_code   | nb_of_times |
        | fail    | 0             | 1           |
        | fail    | 0             | 3           |
        | fail    | 1             | 1           |
        | fail    | 1             | 4           |
