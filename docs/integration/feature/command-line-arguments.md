Command line arguments      {#feature-cmd-args}
======================
\brief Feature documentation on the command line arguments

The various command line options are discussed on their own pages:
- @ref feature-help-option 
- @ref feature-version-option 

# No command line arguments {#feature-cmd-args-empty}
## Scenarios {#scenario-cmd-args-emtpy}
### Scenario: The application is called with no command line arguments and no valid configuration file {#scenario-cmd-args-empty-no-config}
    **GIVEN** A working directory with no valid configuration file
    **WHEN** we call the application without command line arguments
    **THEN** the call should succeed
    **AND THEN** the command line usage information should be printed to stdout

### Scenario: The application is called with no command line arguments and a valid configuration file {#scenario-cmd-args-empty-config}
    **GIVEN** A working directory with a valid configuration file
    **WHEN** we call the application without command line arguments
    **THEN** the call should succeed
    **AND THEN** the command line usage information should be printed to stdout

# Invalid command lines {#feature-cmd-args-invalid}
## Scenarios {#scenario-cmd-args-invalid}
### Scenario: Invalid command line arguments and no configuration file {#scenario-cmd-args-invalid-no-config} 
    **GIVEN** A working directory with no valid configuration file, command line arguments with one or more invalid options and zero or more valid options in any particular order
    **WHEN** we call the application with these command line arguments
    **THEN** the call should not succeed
    **AND THEN** the command line usage information should be printed to stdout

### Scenario: Invalid command line arguments and a configuration file {#scenario-cmd-args-invalid-config} 
    *GIVEN** A working directory with a valid configuration file, command line arguments with one or more invalid options and zero or more valid options in any particular order
    **WHEN** we call the application with these command line arguments
    **THEN** the call should not succeed
    **AND THEN** the command line usage information should be printed to stdout
