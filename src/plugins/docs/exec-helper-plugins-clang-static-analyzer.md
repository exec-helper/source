The clang-static-analyzer plugin  {#exec-helper-plugins-clang-static-analyzer}
=========================
\brief An overview of the clang-static-analyzer plugin configuration settings

# DESCRIPTION
The clang-static-analyzer plugin is used for executing the clang-static-analyzer static code analysis tool.

# MANDATORY SETTINGS
The configuration of the clang-static-analyzer plugin must contain the follwing settings:

## Build-command
The exec-helper build target command or plugin to execute for the analysis.

# OPTIONAL SETTINGS
The configuration of the make plugin may contain the following settings:
 
## command-line
Additional command line parameters to pass to clang-static-analyzer as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include clang-static-analyzer.example

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
