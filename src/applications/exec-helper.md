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
  - It allows to write your system- and project-specific plugins for more advanced optimizations

These optimizations enable efficient users to do what they like to do the most: hang around the coffee machine with peace of mind.

# OPTIONS
Mandatory arguments to long options are mandatory for short options too. Arguments to options can be specified by appending the option with '=ARG' or ' ARG'. This manual will further use the '=ARG' notation. Multiple arguments can be specified, if appropriate and without the need to repeat the option, by using spaces in between the arguments.

\par -h, -\-help
Display a usage message on standard output and exit successfully.

\par -v, -\-verbose
Enable the verbose flag for the command if available.

\par -z, -\-command=COMMAND
Execute one or more configured COMMANDs. This is an alias for the \em \<commands\> mandatory option above.

\par -s, -\-settings-file[=FILE]
Use FILE as the settings file for the exec-helper configuration. Default: \em \.exec-helper.
Exec-helper will use the first file it finds with the given FILE name. It will search in order in the following locations:
1. The current working directory
2. The parent directories of the working directory. The parent directories are searched in _reversed_ order, meaning that the direct parent of the current working directory is searched first, next the direct parent of the direct parent of the current working directory and so-forth until the root directory is reached.
3. The _HOME_ directory of the caller.

\par -j, -\-jobs[=JOBS]
Use the specified number of JOBS for each task (if supported). Use _auto_ to let exec-helper determine an appropriate number. Use a value of _1_ for running jobs single-threaded. Default: _auto_.

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
