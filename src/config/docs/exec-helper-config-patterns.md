Patterns              {#exec-helper-config-patterns}
========
\brief Using patterns in the configuration file

# DESCRIPTION
Patterns are parts of the configuration that will be replaced by its value when evaluated by exec-helper. The _patterns_ keyword describes a list of patterns identified by their key. See the @ref patterns section for more information about how to define a pattern.

Patterns can be used to:
- add options to the exec-helper command line. 
- allow iterating over multiple configurations
- control the configurations to iterate over

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
See [exec-helper-config](@ref exec-helper-config)(5) for information about the exec-helper configuration.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen
