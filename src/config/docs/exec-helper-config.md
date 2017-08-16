Config              {#exec-helper-config}
======
\brief Configuration file formats and conventions for exec-helper

# DESCRIPTION
Exec-helper configuration files are written in the YAML 1.2 specification. 

# MANDATORY KEYS
A valid configuration file must contain at least the following keys on the root level of the configuration file:

## commands
The commands that are configured in the configuration file. It will either contain a list of commands or a list of the commands as keys with an explanation of the command as a value. These formats can not be used interchangeably.

## <command-keys>
For every command defined under the _commands_ key, the configuration must define this command as a key in the root of the configuration file. The value of the key must either be a registered plugin or another command.

## <plugin-keys>
For at least every plugin that is used by a _command_ key, configure the specifics of the plugin (if applicable).

# OPTIONAL KEYS
Optionally the configuration file contains the following keys on the root level of the configuration file:

## patterns
Patterns are parts of the configuration that will be replaced by its value when evaluated by exec-helper. The _patterns_ keyword describes a list of patterns identified by their key. See the @ref exec-helper-config-patterns for more information about how to define a pattern.

# WORKING DIRECTORY
Configured commands are executed from the so-called _working directory_. Executing commands in a different working directory will not affect your current working directory (e.g. when executing from a shell). Each separately configured command can be executed in a separate working directory.

The _working directory_ is the directory that is associated with the first of the following lines whose requirement is met:
1. The _working-dir_ configuration setting is configured for the specific command. The value of the _working-dir_ configuration key can be an absolute path to the working directory or a relative one w.r.t. the directory of the considered configuration file. If the command should be executed in the actual working directory, use _<working-dir>_ as the value in the configuration file.
2. The directory of the considered configuration file.

# EXAMPLE CONFIGURATION
\include exec-helper-config.example

# SEE ALSO
See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5) for more information on defining and using patterns.

See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper.

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for the available plugins and their configuration options.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen

