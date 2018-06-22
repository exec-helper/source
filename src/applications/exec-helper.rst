.. _exec-helper:

exec-helper
***********

Synopsis
========
**exec-helper** <*commands*> [*options*]

**eh** <*commands*> [*options*]

Description
===========
The :program:`exec-helper` utility is a meta-wrapper for executables, optimizing one of the slowest links in most workflows: you. It enables the user to optimize the existing workflow in multiple minor and major ways:

* It minimizes the amount of typing while eliminating redundancies
* It chains multiple commands, inserting patterns at specified places
* It avoids having to memorize or search for the right invocations for more complicated commands
* It allows to write your system- and project-specific plugins for more advanced optimizations

These optimizations enable efficient users to do what they like to do the most: hang around the coffee machine with peace of mind.

Options
=======
Mandatory arguments to long options are mandatory for short options too. Arguments to options can be specified by appending the option with '=ARG' or ' ARG'. This manual will further use the '=ARG' notation. Multiple arguments can be specified, if appropriate and without the need to repeat the option, by using spaces in between the arguments.

.. program:: exec-helper

.. option:: -h, --help

    Display a usage message on standard output and exit successfully.

.. option:: -v, --verbose

    Enable the verbose flag for the command if available.

.. option:: -z, --command=COMMAND

    Execute one or more configured COMMANDs. This is an alias for the *<commands>* mandatory option above.

.. option:: -s, --settings-file[=FILE]

    Use FILE as the settings file for the :program:`exec-helper` configuration. Default: *.exec-helper*.
    Exec-helper will use the first file it finds with the given FILE name. It will search in order in the following locations:

    1. The current working directory
    2. The parent directories of the working directory. The parent directories are searched in *reversed* order, meaning that the direct parent of the current working directory is searched first, next the direct parent of the direct parent of the current working directory and so-forth until the root directory is reached.
    3. The *HOME* directory of the caller.

.. option:: -j, --jobs[=JOBS]

    Use the specified number of JOBS for each task (if supported). Use *auto* to let :program:`exec-helper` determine an appropriate number. Use a value of *1* for running jobs single-threaded. Default: *auto*.

.. option:: -n, --dry-run

    Print the commands that would be executed, but  do not execute them.

Configured options
==================
Additional command-line options for :program:`exec-helper` can be configured in the settings file. Refer to the :manpage:`exec-helper-config(5)` documentation for more information. 

Exit status
===========
Exec-helper exits with a status of *zero* if all commands are executed successfully. When :program:`exec-helper` is called improperly or its plugins are invoked improperly, :program:`exec-helper` will exit with a status of *one*. If the executed return commands return an error code, the *exit status of the last executed command* is returned as the exit status of :program:`exec-helper`.

Auto-completion
===============
Auto-completions are available for the Bash and Zsh shell. Package maintainers receive the tools to automatically enable these completions. If your installation package does not do this, you can enable them yourself by adding `source <install-directory>/share/exec-helper/completions/init-completion.sh` to your profile or bashrc.

See also
========
See :ref:`exec-helper-config` (5) for information about the configuration file.

See :ref:`exec-helper-plugins` (5) for the available plugins and their configuration options.
