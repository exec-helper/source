An overview of the valgrind plugin configuration settings {#exec-helper-plugins-valgrind}
==========================================================

# DESCRIPTION
The valgrind plugin is used for executing code coverage analysis using valgrind.

# MANDATORY SETTINGS
The configuration of the valgrind plugin must contain the following settings:

## run-command
The exec-helper command or plugin to use for running the binaries which need to be analyzed.

# OPTIONAL SETTINGS
The configuration of the valgrind plugin may contain the following settings:

## tool
The valgrind tool to use. Default: the _tool_ is omitted.

## command-line
Additional command line parameters to pass to valgrind as a list of separate arguments. Default: an empty list.

# EXAMPLE
## Configuration
\include valgrind.example

## Additional files
In order for the above example to work, the following file hierarchy needs to be created in the directory:

_Makefile_:
\include Makefile

_src/hello.cpp:_
\include hello.cpp

_src/world.cpp:_
\include world.cpp

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
