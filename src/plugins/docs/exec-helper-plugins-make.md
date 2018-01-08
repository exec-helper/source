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
A list of _patterns_ to apply on the command line. See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5).
 
## enviroment
A list of _environment variables_ that should be set before the commands are executed. See [exec-helper-config-environment](@ref exec-helper-config-environment)(5).

## build-dir
The path to the Makefile. This is either an absolute path are a path relative to the location of this file. Default: \em . (the current working directory).

## command-line
Additional command line parameters to pass to make as a list of separate arguments. By default no additional arguments are added.

## working-dir
The working directory of the command. Can be an absolute path are a relative one w.r.t. the path to the considered configuration file. Commands that should be executed relative to the current working dir can use the {EH\_WORKING\_DIR} pattern.

# EXAMPLE
## Configuration
\include make.example

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
