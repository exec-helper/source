The command-line-command plugin  {#exec-helper-plugins-command-line-command}
============================
\brief An overview of the command-line-command plugin configuration settings

# DESCRIPTION
The command-line-command is used for executing arbitrary command lines. This plugin can be used for constructing the command line for commands that do not have a corresponding plugin available.

# MANDATORY SETTINGS
The configuration of the command-line-command must contain the following settings:

## command-line
The command-line to execute. There are two different usages:
- _No identification key_: Set one command line as a list of separate arguments. This form is only usable if only one line needs to be executed.
- _With identification key_: Make a map with arbitrary keys, where each associated value is one command line, described as a list of separate arguments. This form is usable if one or more lines need to be executed. Multiple commands are executed in the order the identification keys are defined.
  
Note: see the documentation of [wordexp](@ref wordexp)(3) for the limitations on what characters are not allowed in the command-line command.

# OPTIONAL SETTINGS
The configuration of the command-line-command plugin may contain the following settings:
 
## patterns
A list of \em patterns to apply on the command line. See [exec-helper-patterns](@ref exec-helper-patterns)(5).
 
## env
A list of _environment variables_ that should be set before the commands are executed. See [exec-helper-env](@ref exec-helper-env)(5).

# EXAMPLE
## Configuration
\include command-line-command.example

## Usage
Save the example to an exec-helper configuration file and execute in the same directory:
\code{.sh}
eh example
\endcode

# SEE ALSO
See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper.

See [exec-helper-config](@ref exec-helper-config)(5) for information about the configuration file format.

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for information about the configuration file format.

# COPYRIGHT
 Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen
