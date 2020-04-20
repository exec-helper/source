Environment              {#exec-helper-config-environment}
===========
\brief Using environment variables in the configuration file

# DESCRIPTION
Environment variables can be configured in the configuration file. They will only be set for the particular command(s) defined by the relevant section of the configuration.

Environment variables can _not_ be set directly in a command line command. The _environment_ configuration key needs to be used for this. See section @ref environment.

# ENVIRONMENT      {#environment}
The _environment_ keyword can be set for every plugin that supports the env configuration setting. Check the documentation on a specific plugin to check whether the plugin supports this configuration setting.

The _environment_ keyword must contain a _map_ of key-value pairs, where the key is the name of the environment variable and the value is the value associated with the specified environment variable. [Patterns](@ref exec-helper-config-patterns) can not be used for the environment variable _value_.

_Note_: The _PWD_ environment variable, following POSIX convention, is set by the application to the working directory of the task. Therefore, its value cannot be overriden in the configuration.

# EXAMPLE CONFIGURATION
\include exec-helper-config-environment.example

# SEE ALSO
See [exec-helper-config](@ref exec-helper-config)(5) for information about the exec-helper configuration.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2020 under the Berkeley Software Distribution 3-clause (BSD 3-clause).

# AUTHOR
Written by B. Verhagen
