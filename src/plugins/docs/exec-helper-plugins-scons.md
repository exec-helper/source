The scons plugin  {#exec-helper-plugins-scons}
===============
\brief An overview of the scons plugin configuration settings

# DESCRIPTION
The scons plugin is used for executing scons.

# MANDATORY SETTINGS
There are no mandatory settings for this plugin.

# OPTIONAL SETTINGS
The configuration of the scons plugin may contain the following settings:
 
## patterns
A list of _patterns_ to apply on the command line. See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5).
 
## command-line
Additional command line parameters to pass to the scons as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include scons.example

## Additional files
In order for the above example to work, the following file hierarchy needs to be created in the directory:

_src/hello.cpp:_
\include hello.cpp

_src/world.cpp:_
\include world.cpp

_SConstruct:_
\include SConstruct

_SConscript:_
\include SConscript

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
