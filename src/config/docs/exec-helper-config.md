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
Patterns are parts of the configuration that will be replaced by its value when evaluated by exec-helper. The _patterns_ keyword describes a list of patterns identified by their key. See the @ref patterns section for more information about how to define a pattern.

# PATTERNS      {#patterns}
A pattern can contain the following fields:
## default-values
A list of default values to use when no values have been defined.

## short-option
The short option on the command line associated with this pattern

## long-option
The long option on the command line associated with this pattern

# EXAMPLE CONFIGURATION
\include exec-helper-config.example

# SEE ALSO
See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for the available plugins and their configuration options.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen

