An overview of the cppcheck plugin configuration settings {#exec-helper-plugins-cppcheck}
=========================================================

# DESCRIPTION
The cppcheck plugin is used for executing the cppcheck static code analysis tool.

# MANDATORY SETTINGS
There are no mandatory settings for the cppcheck plugin.

# OPTIONAL SETTINGS
The configuration of the cppcheck plugin may contain the following settings:

## patterns
A list of _patterns_ to apply on the command line. See [exec-helper-patterns](@ref exec-helper-patterns)(5).

## enable-checks
A list of checks that should be enabled or disabled. Check the documentation of cppcheck for a list of all the available checks. Default: _all_.

## src-dir
The base directory containing all the files to check. Default: \em . (the current working directory).

## command-line
Additional command line parameters to pass to cppcheck as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include cppcheck.example

## Additional files
In order for the above example to work, the following files need to be created in the _src_ directory:

_hello.cpp:_
\include hello.cpp

_world.cpp:_
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
