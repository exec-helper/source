Execute multiple statements {#execute-multiple-statements}
===========================
\brief Feature documentation on executing multiple statements

Feature: execute multiple predefined statements in a predefined order
---------------------------------------------------------------------
### Configuration
Predefined orders:
- are configured as a _command_
- should be easy and clearly expressed in the configuration file
- should be easy to trigger

### Command execution
Predefined orders:
- should immediately exit when an executed statement has failed, unless _explicitly_ configured differently
- should exit with the same return code as the last executed command

Execute multiple predefined statements in a specified order at runtime
----------------------------------------------------------------------
### Configuration
Ordered statements at runtime:
- are expressed as an ordered sequence of _commands_
- should immediately exit when an executed command has failed, unless _explicitly_ configured differently
- should exit with the same error code as the last executed _command_
