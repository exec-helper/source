Plugins                 {#exec-helper-plugins}
=======
\brief The list of plugins for exec-helper

# DESCRIPTION
This document describes the list of plugins that can be used in the associated exec-helper binaries.

# GENERAL PLUGINS
## command-line-command
The _command-line-command_ plugin is used for executing arbitrary command line commands.
See [exec-helper-plugins-command-line-command](@ref exec-helper-plugins-command-line-command)(5).

## command-plugin
The _command plugin_ is used for executing the given command as it is configured in the exec-helper configuration file. See [exec-helper-plugins-command-plugin](@ref exec-helper-plugins-command-plugin)(5).

## execute-plugin
The _execute plugin_ is used for executing other commands and/or targets configured in the exec-helper configuration file. See [exec-helper-plugins-execute-plugin](@ref exec-helper-plugins-execute-plugin)(5).

## memory
The _memory plugin_ remembers all calls and state the plugin was called with. This plugin is mainly useful for testing and debugging purposes. See [exec-helper-plugins-memory](@ref exec-helper-plugins-memory)(5).

## selector
The _selector plugin_ is used for selecting certain configuration paths based on the value of a pattern. See [exec-helper-plugins-selector](@ref exec-helper-plugins-selector)(5).

# BUILD PLUGINS
## bootstrap
The _bootstrap plugin_ is used for calling bootstrap scripts, typically used as a step in a build chain. See [exec-helper-plugins-bootstrap](@ref exec-helper-plugins-bootstrap)(5).

## make
The _make plugin_ is used for running the make build system. See [exec-helper-plugins-make](@ref exec-helper-plugins-make)(5).

## scons
The _scons plugin_ is used for running the scons build system. See [exec-helper-plugins-scons](@ref exec-helper-plugins-scons)(5).

# ANALYSIS PLUGINS
## clang-static-analyzer
The _clang-static-analyzer plugin_ is used for applying the clang static analyzer tool on source code files. See [exec-helper-plugins-clang-static-analyzer](@ref exec-helper-plugins-clang-static-analyzer)(5).

## clang-tidy
The _clang-tidy plugin_ is used for applying the clang tidy tool on source code files. See [exec-helper-plugins-clang-tidy](@ref exec-helper-plugins-clang-tidy)(5).

## cppcheck
The _cppcheck plugin_ is used for applying cppcheck on source code files. See [exec-helper-plugins-cppcheck](@ref exec-helper-plugins-cppcheck)(5).

## lcov
The _lcov plugin_ is used for applying the lcov code coverage analysis tool. See [exec-helper-plugins-lcov](@ref exec-helper-plugins-lcov)(5).

## pmd
The _pmd plugin_ is used for applying pmd analysis on source code files. See [exec-helper-plugins-pmd](@ref exec-helper-plugins-pmd)(5).

## valgrind
The _valgrind plugin_ is used for applying valgrind analysis. See [exec-helper-plugins-valgrind](@ref exec-helper-plugins-valgrind)(5).

# SEE ALSO
See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper.

See [exec-helper-config](@ref exec-helper-config)(5) for information about the configuration file format.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen

