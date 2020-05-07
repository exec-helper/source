The CMake plugin  {#exec-helper-plugins-cmake}
===============
\brief An overview of the cmake plugin configuration settings

# DESCRIPTION
The cmake plugin is used for generating, building and installing software using the CMake build generator system.

# MANDATORY SETTINGS
There are no mandatory settings for this plugin, though it is recommended to configure the _mode_ setting explicitly.

# OPTIONAL SETTINGS
The configuration of the make plugin may contain the following settings:

## Settings for all modes

### mode
Set the mode of the CMake call for the specific command. Default: _generate_.

Supported modes are:

- _Generate_: For generating a build directory based on the CMake configuration in the source. This is often callend the _configure_ or _build init_ step.
- _Build_: Build the generated project
- _Install_: Install the generated project
 
### patterns
A list of _patterns_ to apply on the command line. See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5).
 
### enviroment
A list of _environment variables_ that should be set before the commands are executed. See [exec-helper-config-environment](@ref exec-helper-config-environment)(5).

### build-dir
The path to the build directory. This is either an absolute path are a path relative to the location of this file. Default: \em . (the directory of the exec-helper configuration).

### command-line
Additional command line parameters to pass to make as a list of separate arguments. By default no additional arguments are added.

### working-dir
The working directory of the command. Can be an absolute path are a relative one w.r.t. the path to the considered configuration file. Commands that should be executed relative to the current working dir can use the {EH\_WORKING\_DIR} pattern.

## Settings for the _generate_ mode
### source-dir
The directory containing the root CMakeLists.txt file of the sources. Default: \em . (the directory of the exec-helper configuration).

### generator
The generator to use for generating the build directory. See the CMake documentation on which generators are supported for your platform and the value(s) to explicitly set them. Default: the default one for your system and environment. See the CMake documentation on the details.

### defines
A map of the _build generator settings_ for configuring the generator.

## Settings for the _build_ mode
### target
The specific CMake target to build. Default: the default target. See the CMake documentation for more details.

### config
The configuration for multi-configuration tools. Default: the default configuration. See the CMake documentation for more details.

## Settings for the _install_ mode
### config
The configuration for multi-configuration tools. Default: the default configuration. See the CMake documentation for more details.

### prefix
Override the configured prefix set during the _generate_ mode. Default: the default installation prefix. See the CMake documentation for more details.

### component
Limit installation to the given component. Default: all installation targets.

# EXAMPLE
## Configuration
\include cmake.example

## Additional files
In order for the above example to work, the following file hierarchy needs to be created in the directory:

_CMakeLists.txt_:
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
Exec-helper is Copyright &copy; 2016-2020 under the Berkeley Software Distribution 3-clause (BSD 3-clause).

# AUTHOR
Written by B. Verhagen
