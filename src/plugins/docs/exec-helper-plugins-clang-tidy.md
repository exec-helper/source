The clang-tidy plugin  {#exec-helper-plugins-clang-tidy}
=====================
\brief An overview of the clang-tidy plugin configuration settings

# DESCRIPTION
The clang-tidy plugin is used for executing the clang-tidy static code analysis tool.

# MANDATORY SETTINGS
There are no mandatory settings for the clang-tidy plugin.

# OPTIONAL SETTINGS
The configuration of the clang-tidy plugin may contain the following settings:

## sources
A list of sources that must be checked by the clang-tidy plugin. The sources may contain wildcards.

## checks
A list of checks that should be enabled or disabled. Enabling or disabling checks is done the same way as they are enabled on the clang-tidy command line. Default: no checks will be enabled or disabled on the command line, meaning the default checks enabled by clang will be checked.

## warning-as-errors
Threat warnings as errors.
The value associated with this key is either:
  -# A list of checks: for which warnings will be threated as errors. See @ref checks for the format.
  -# The single keyword _all_: means that all enabled checks will be threated as errors.

_Note_: This options is only supported if the clang-tidy binary supports the _-warnings-as-error=<string>_ option.
 
## command-line
Additional command line parameters to pass to clang-tidy as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include clang-tidy.example

## Additional files
In order for the above example to work, the following files need to be created in the same directory as well:

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
