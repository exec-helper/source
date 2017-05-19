The make plugin  {#exec-helper-plugins-make}
===============
\brief An overview of the make plugin configuration settings

# DESCRIPTION
The make plugin is used for executing Makefiles.

# MANDATORY SETTINGS
There are no mandatory settings for this plugin.

# OPTIONAL SETTINGS
The configuration of the make plugin may contain the following settings:
 
## patterns
A list of _patterns_ to apply on the command line. See [exec-helper-patterns](@ref exec-helper-patterns)(5).
 
## env
A list of _environment variables_ that should be set before the commands are executed. See [exec-helper-env](@ref exec-helper-env)(5).

## build-dir
The path to the Makefile. This is either an absolute path are a path relative to the location of this file. Default: \em . (the current working directory).

## command-line
Additional command line parameters to pass to make as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include make.example

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
