The pmd plugin  {#exec-helper-plugins-pmd}
==============
\brief An overview of the pmd plugin configuration settings

# DESCRIPTION
The pmd plugin is used for executing Makefiles.

# MANDATORY SETTINGS
The configuration of the pmd plugin must contain the following settings:

# OPTIONAL SETTINGS
The configuration of the pmd plugin may contain the following settings:
 
## patterns
A list of _patterns_ to apply on the command line. See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5).

## exec
The path to the pmd-run executable. The path can either be an absolute path or a relative path from the current working directory. Default: _pmd_.

## tool
The pmd tool to use. The currently supported tools are:
  - cpd
Default: _cpd_

## language
Specify the language PMD is analyzing. Check the _-language_ option of the pmd documentation for more information. Default: no explicit language parameter is passed.

## command-line
Additional command line parameters to pass to pmd as a list of separate arguments. By default no additional arguments are added.

## CPD SPECIFIC SETTINGS
### minimum-tokens
The minimum token length to be considered a duplicate. Check the _--minimum-tokens_ option of the cpd documentation for more information. Default: no explicit minimum tokens parameter is passed.

### files
A list of files to check for duplicated code. Check the _--files_ option of the cpd documentation for more information. Default: no explicit files parameter is passed.

# EXAMPLE
## Configuration
\include pmd.example

## Additional files
In order for the above example to work, the following file hierarchy needs to be created in the directory:

_Makefile_:
\include Makefile

_src/hello.cpp:_
\include hello.cpp

_src/world.cpp:_
\include world.cpp

## Dependencies
  - PMD must be installed.
  - The pmd-run or run.sh script must be present in your path.

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
