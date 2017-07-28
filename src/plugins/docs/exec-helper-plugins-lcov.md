The lcov plugin  {#exec-helper-plugins-lcov}
===============
\brief An overview of the lcov plugin configuration settings

# DESCRIPTION
The lcov plugin is used for executing code coverage analysis using lcov.

# MANDATORY SETTINGS
The configuration of the lcov plugin must contain the following settings:

## run-command
The exec-helper command or plugin to use for running the binaries for which the coverage needs to be analyzed.

# OPTIONAL SETTINGS
The configuration of the lcov plugin may contain the following settings:

## info-file
The lcov .info file to use for the analysis. Default: _lcov-plugin.info_.

## base-directory
The base directory to use for the lcov analysis. Check the lcov documentation on the _--base-directory_ option for more information. Default: \em . (the current working directory).

## directory
Use the coverage data files in the given directory. Check the lcov documentation on the _--directory_ option for more information. Default: \em . (the current working directory).

## zero-counters
Set this option to _yes_ to reset the coverage counters before starting the analysis. All other values are threated as _no_. Default: _no_.

## gen-html
Set this option to _yes_ to enable HTML report generation of the coverage data.

## gen-html-output
Set the output directory of the generated HTML report. Does nothing if __gen-html__ is not enabled. Default: \em . (the current working directory).

## gen-html-title
Set the title of the generated HTML report. Does nothing if __gen-html__ is not enabled. Default: _Hello_.

## excludes
A list of directories and files to excluse from the coverage report. The paths are relative to the current working directory. Default: an empty list.
 
## command-line
Additional command line parameters to pass to lcov as a list of separate arguments. Default: an empty list.

# EXAMPLE
## Configuration
\include lcov.example

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
