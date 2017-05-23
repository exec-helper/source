The bootstrap plugin  {#exec-helper-plugins-bootstrap}
====================
\brief An overview of the bootstrap plugin configuration settings

# DESCRIPTION
The bootstrap is used for executing bootstrap files. This is often used in build chains.

# MANDATORY SETTINGS
There are no mandatory settings for the bootstrap plugin.

# OPTIONAL SETTINGS
The configuration of the bootstrap plugin may contain the following settings:
 
## patterns
A list of \em patterns to apply on the command line. See [exec-helper-patterns](@ref exec-helper-patterns)(5).
 
## filename
The name of the bootstrap script. Default: _bootstrap.sh_.

## command-line
Additional command line parameters to pass to the bootstrap script as a list of separate arguments. By default no additional arguments are added.

# EXAMPLE
## Configuration
\include bootstrap.example

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