Execute multiple statements {#feature-execute-multiple-statements}
===========================
\brief Feature documentation on executing multiple statements

# Execute multiple predefined statements in a predefined order {#feature-execute-multiple-statements-predefined-order}
## Configuration {#predefined-order-configuration}
Predefined orders:
-# are configured as a _command_
-# should be easy and clearly expressed in the configuration file
-# should be easy to trigger

## Command execution {#predefined-order-command-execution}
Predefined orders:
-# should trigger the statements associated with a command defined at runtime the _expected number of times_
-# should immediately exit when an executed statement has failed, unless _explicitly_ configured differently
-# should exit with the same return code as the last executed command

## Scenarios
### Scenario: Execution order of a command consisting of one predefined statement {#scenario-execute-multiple-statements-predefined-order-single}
   **GIVEN** A valid configuration with multiple commands consisting of one predefined, valid, successful statement
    **WHEN** we call each of these commands separately
    **THEN** the call should succeed (@ref predefined-order-command-execution)
**AND THEN** the associated predefined statement should be called exactly once (@ref predefined-order-command-execution)
**AND THEN**: no other predefined statements should have been called (@ref predefined-order-command-execution)

### Scenario: Execution order of commands consisting of multiple predefined statements {#scenario-execute-multiple-statements-predefined-order-multiple}
   **GIVEN**: A valid configuration with multiple commands consisting of multiple, predefined, valid, successful statements
    **WHEN**: we call each of these commands separately
    **THEN**: the call should succeed (@ref predefined-order-command-execution)
**AND THEN**: the associated predefined statements should be called the expected number of times in the right order (@ref predefined-order-command-execution)
**AND THEN**: no other predefined statements should have been called (@ref predefined-order-command-execution)

### Scenario: Duplicate configured statements {#scenario-execute-multiple-statements-predefined-order-duplicates}
   **GIVEN**: A valid configuration with a command consisting of multiple, duplicate statements and some non-duplicate ones
    **WHEN**: we call this command
    **THEN**: the call should succeed (@ref predefined-order-command-execution)
**AND THEN**: the associated predefined statements should all be called the expected number of times (@ref predefined-order-command-execution)
**AND THEN**: no other predefined statements should have been called (@ref predefined-order-command-execution)

### Scenario: Failing statements {#scenario-execute-multiple-statements-predefined-order-failing-statements}
   **GIVEN**: A valid configuration with a command consisting of multiple, predefined, valid, successful statements and a failing statement that is not the first nor the last statement
    **WHEN**: we call this command
    **THEN**: the call should fail (@ref predefined-order-command-execution)
**AND THEN**: the call should exit with the same return code as the failed statement (@ref predefined-order-command-execution)
**AND THEN**: the statements configured before the failed statement and the failed statements should have been executed (@ref predefined-order-command-execution)
**AND THEN**: no other predefined statements should have been called (@ref predefined-order-command-execution)

# Execute multiple predefined statements in a specified order at runtime {#feature-execute-multiple-statements-order-at-runtime}
## Configuration {#order-at-runtime-configuration}
Ordered statements at runtime:
-# are expressed as an ordered sequence of _commands_
-# should be easy to construct and modify at runtime

## Command execution {#order-at-runtime-command-execution}
Ordered statements at runtime:
-# should only call the commands defined at runtime
-# should immediately exit when an executed command has failed, unless _explicitly_ configured differently
-# should exit with the same error code as the last executed _command_

## Scenarios
### Scenario: Execution order of a single command {#scenario-execute-multiple-statements-order-at-runtime-single}
   **GIVEN**: A valid configuration file with a predefined, valid, successful command
    **WHEN**: we call the command
    **THEN**: the call should succeed (@ref order-at-runtime-command-execution}
**AND THEN**: the statements of the command should have been called in the right order (@ref order-at-runtime-command-execution)
**AND THEN**: no other commands should have been called (@ref order-at-runtime-command-execution)

### Scenario: Execution order of multiple commands {#scenario-execute-multiple-statements-order-at-runtime-multiple}
   **GIVEN**: A valid configuration file with multiple predefined, valid, successful commands
    **WHEN**: we call the commands, including duplicate ones, in a certain order
    **THEN**: the call should succeed (@ref order-at-runtime-command-execution)
**AND THEN**: the statements associated with each command should have been called in the right order (@ref order-at-runtime-command-execution)
**AND THEN**: no other commands should have been called (@ref order-at-runtime-command-execution)

### Scenario: Failing commands {#scenario-execute-multiple-statements-order-at-runtime-failing-commands}
   **GIVEN**: A valid configuration file with a mix of multiple predefined, valid, failing and successful commands
    **WHEN**: we call the commands
    **THEN**: the call should fail (@ref order-at-runtime-command-execution)
**AND THEN**: the call should exit with the same return code as the failed statement of the failed command (@ref order-at-runtime-command-execution)
**AND THEN**: the commands configured to run before the failed command, the failing command, the statements configured to execute before the failing statement and the failing statement should have been executed the expected number of times (@ref order-at-runtime-command-execution)
**AND THEN**: no other commands or statements should have been called (@ref order-at-runtime-command-execution)

# Glossary
- _statements_: One command line instruction as it would be executed when executed manually.
- _expected number of times_: The number of times the statement is encountered when following the configured path for one or more executed commands.
