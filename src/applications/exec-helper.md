exec-helper, eh          {#exec-helper}
===============
\brief Or How To Get Your Coffee In Peace: A meta-wrapper for executables

# SYNOPSIS
exec-helper \<commands\> [options]

eh \<commands\> [options]

# DESCRIPTION
The _exec-helper_ utility is a meta-wrapper for executables, optimizing one of the slowest links in most workflows: you. It enables the user to optimize the existing workflow in three major ways:
  - It minimizes the amount of typing while eliminating redundancies
  - It chains multiple commands, inserting patterns at specified places
  - It avoids having to memorize or search for the right invocations for more complicated commands

These optimizations enable efficient users to do what they like to do the most: hang around the coffee machine with a peace of mind.

# OPTIONS
Mandatory arguments to long options are mandatory for short options too. Arguments to options can be specified by appending the option with '=ARG' or ' ARG'. This manual will further use the '=ARG' notation. Multiple arguments can be specified, if appropriate and without the need to repeat the option, by using spaces in between the arguments.

\par -h, -\-help
Display a usage message on standard output and exit successfully.

\par -v, -\-verbose
Enable the verbose flag for the command if available.

\par -z, -\-command=COMMAND
Execute one or more configured COMMANDs. This is an alias for the \em \<commands\> mandatory option above.

\par -s, -\-settings-file[=FILE]
Use FILE as the settings file for the exec-helper configuration. By default exec-helper searches for a \em \.exec-helper file in the current directory and the _$HOME_ directory of the caller.

\par -u, -\-single-threaded
Enable the single-threaded flag for the command if available.

\par -n, -\-dry-run
Print the commands that would be executed, but  do not execute them.

# CONFIGURED OPTIONS
Additional command-line options for exec-helper can be configured in the settings file. Refer to the _exec-helper-config_ documentation for more information. 

# EXIT STATUS
Exec-helper exits with a status of _zero_ if all commands are executed successfully. When exec-helper is called improperly or its plugins are invoked improperly, exec-helper will exit with a status of _one_. If the executed return commands return an error code, the _exit status of the last executed command_ is returned as the exit status of exec-helper.

# SEE ALSO
See [exec-helper-config](@ref exec-helper-config)(5) for information about the configuration file.

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for the available plugins and their configuration options.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2017 under the GNU General Public License v3.0.

# AUTHOR
Written by B. Verhagen
