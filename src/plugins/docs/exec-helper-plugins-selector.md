The selector plugin  {#exec-helper-plugins-selector}
===================
\brief An overview of the selector plugin configuration settings

# DESCRIPTION
The selector plugin is used for selecting a configuration path based on the value(s) of a pattern.

# MANDATORY SETTINGS
The configuration of the command-line-command must contain the following settings:

## patterns
The pattern keys to use for selecting the configuration path. The runtime value(s) associated with the pattern key must resolve either to an existing (configured) plugin or a configured command.

# OPTIONAL SETTINGS
There are no optional settings for the selector plugin.
 
# EXAMPLE
## Configuration
\include selector.example

# SEE ALSO
See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper.

See [exec-helper-config](@ref exec-helper-config)(5) for information about the configuration file format.

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for information about the configuration file format.

# COPYRIGHT
 Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen
