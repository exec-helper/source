.. _exec-helper-custom-plugins:

Custom plugins
**************

Where to put your plugins
=========================
Exec-helper searches dynamically for (most of) its plugins in all the plugin search paths. It searches in the following locations (earlier listed locations take precedence over later listed locations for plugins with the same name):

1. Using the :code:`--additional-search-path` command-line option. Multiple paths can be passed to it using multiple arguments. Earlier mentioned paths take precedence over later mentioned paths. The paths can be absolute or relative w.r.t. the used :program:`exec-helper` configuration file. E.g.::

    exec-helper build --additional-search-path blaat /tmp

will add the relative path *blaat* and the absolute path */tmp* to the plugin search paths.

2. Using the :code:`additional-search-paths` key in the :program:`exec-helper` configuration file. The key takes an ordered list containing absolute or relative (w.r.t. the :program:`exec-helper` configuration file it is mentioned in) paths. Earlier listed elements take precedence over lower listed elements. E.g.::

    additional-search-paths:
        - blaat
        - /tmp

3. The system plugin paths. These paths contain (most of) the default modules bundled with :program:`exec-helper`. It is not recommended to add your custom plugins to any of these paths.

Listing the modules
===================
Exec-helper lists the modules it currently finds by using the :code:`--list-plugins` command-line option.

Writing a lua plugin
====================
Exec-helper supports luaJIT 2.0.5. LuaJIT is a Lua 5.1 implementation with some additional features from Lua 5.2. All LuaJIT functionality is embedded in the :program:`exec-helper` binary, no LuaJIT install is required for running the plugin.

Exec-helper treats all files in the plugin search paths with a *lua* suffix as a compatible lua plugin. The name of the module is derived from the rest of the filename.

The interface
=============
A lua plugin is called within a wider (lua) context containing some objects and (convenience) functions.

Exec-helper specific functions
------------------------------
The following :program:`exec-helper` specific functions are available next to the lua 5.1 functions:

.. highlight:: lua

.. describe:: get_commandline()

    Returns a list of the command-line arguments set by the :code:`command-line` key in the configuration. Use this to allow users of your plugin to freely set additional, plugin-specific command-line settings that can not be set by other configuration options. These additional command-line settings must be added explicitly by this plugin in the right position. E.g::

        task:add_args(get_commandline())

.. describe:: get_environment()

    Returns a two-level Lua table containing the environment in which the task will be executed. The plugin can read and modify this environment. Values set by the `environment` key in the configuration are added automatically to this list before this plugin is called, there is no need to do this explicitly.

    **Note**: The *PWD* environment variable, following POSIX convention, is set by the application to the working directory of the task. Therefore, its value cannot be overriden in a custom module.

.. describe:: get_verbose(string arg)

    Add :code:`arg` to the current tasks' command line if verbose mode is activated. This function does nothing if verbose mode is not activated. E.g.::

        task:add_args(get_verbose('--debug'))

.. describe:: register_task(Task task)

    Registers the given :code:`task` as a task to execute by the executor(s). Patterns associated with the task will be automatically permutated and substituted. E.g.::

        register_task(task)

.. describe:: register_tasks(array<Task> tasks)

    Registers the given :code:`tasks` as multiple tasks to execute by the executor(s). Patterns associated with the task will be automatically permutated and substituted. E.g.::

        register_tasks(tasks)

.. describe:: run_target(Task task, array<string> targets)

    Applies the given targets using the given task as their base task. These targets may contain patterns. The result of these applications is returned as an :code:`array<Task>`. The returned tasks must be explicitly registered in order to be executed. E.g.::

        run_target(task, {'cmake', 'ninja'})

.. describe:: user_feedback_error(string message)

    Show the given :code:`message` as an error to the user. E.g.::

        user_feedback_error('You should not do that!')

.. describe:: input_error(string message)

    Show the given :code:`message` as an error to the user and stop execution of this module. E.g.::

        input_error('Cowardly refusing to perform that action!')

Exec-helper specific types
--------------------------
The following types (classes) are available in your module:

.. describe:: Config

    Behaves like an ordinary lua table. Only reading from it using the access operator (`[key]`) is allowed. The access operator takes a string and returns a Lua table.

.. describe:: Task

    Contains the task that is being built. It has the following member functions:

    * :code:`add_args(array<string> args)`: Append the given arguments to this task.
    * :code:`new(Task task)`: Create a new, default task with an empty command line.
    * :code:`copy(Task task)`: Returns a copy of the given task.


Pre-defined objects
-------------------
The following pre-defined objects are automatically present when your module is called:

.. describe:: verbose

    A boolean indicating whether the verbose command-line flag was set for this invocation.

.. describe:: jobs

    Integer indicating the number of jobs to use for executing this plugin, if the plugin supports parallel job execution. Ignore this if this is not the case.

    Example::

        task:add_args({'--jobs', jobs})

    Adds :code:`--jobs \<value\>` to the command line of the given task where `<value>` is the value of the configured number of jobs.

.. describe:: config

    A pure Lua table containing the configuration of the particular :program:`exec-helper` configuration into one easy-to-navigate syntax tree. The tree may contain multiple levels. Accessing a table value in Lua returns a new Lua table. Use the :code:`one()` and :code:`list()` function to convert the table to a single value or list respectively. These functions will return `nil` when the given key has no value. The functions distinguish between no value (`nil`) and an empty value (e.g. an empty list).

    Example::

        task:add_args({'--directory', one(config['build-dir']) or '.'})

    Adds :code:`--directory \<value\>` to the task command line, where `<value>` is one value set by the `build-dir` key or `.` when no such key exists in the configuration of this plugin.

.. describe:: task

    A Task object containing the current context for executing the task, this may include prefixes from other plugins. It is *not* possible to erase these prefixes. If your module requires pre- or post-tasks, you can create one or more new tasks and register it. Similarly, it is possible to create new tasks with the same context as the given *task* variable by copy constructing it. Use the Lua *:* operator for calling member functions of a task.

    For example, to create a module that calls `echo hello` on its invocation, use::

        task:add_args({'echo', 'hello'})

Example
=======
A module for a directly callable tool
-------------------------------------
Let's implement a simple module for calling `make` called :code:`make`:

*make.lua*:

.. literalinclude:: ../src/scripts/make.lua
    :language: lua

This module adds `make` with some additional arguments from the config and the options to the existing :code:`task` task. At the end, it registers the task for execution.

The relevant section in the users' :program:`exec-helper` configuration may look like:

.. code-block:: yaml

    commands:
        build: Build the project

    patterns:
        MODE:
            default-values:
                - debug
                - release
            short-option: m
            long-option: mode

    build:
        - make

    make:
        patterns:
            - MODE

        build:
            build-dir: "build/{MODE}"
            jobs: 3
            command-line: [ --dry-run, --keep-going] 

Running :code:`eh build --mode release --verbose` will execute the command-line:

.. code-block:: bash

    make --directory build/release --debug --jobs 3 --dry-run --keep-going

A module calling an other command
---------------------------------
Let's implement a simple module for `clang-static-analyzer`. Per the docs, this analyzer is used by prepending :code:`scan-build <options> <build command>` to the build command line. Obviously, users will already have configured a command (e.g. `build`) for building the project without any analysis. For maintenance and convenience purposes, we do not want the user to replicate this build command for this plugin, but rather, we want our plugin to add some arguments to the tasks' command line and call the configured build-command for extending the task with the actual build configuration.

Let's implement this module, called under the name :code:`some-analyzer`:

*some-analyzer.lua*:

.. literalinclude:: ../src/scripts/clang-static-analyzer.lua
    :language: lua

This module adds :code:`scan-build` and some additional arguments to the command line of the task. Next, it takes the :code:`build-command` configuration values, does some validity checks on it, and requests :program:`exec-helper` to extend the command with the arguments of the given command values.

The relevant section in the users' :program:`exec-helper` configuration (combined with the module above for implementing the build command) may look like:

.. code-block:: yaml

    build:
        - make

    make:
        build-dir: build
        
    some-analyzer:
        build-command: build
        command-line:
            - --keep-going

Running :code:`eh some-analyzer --jobs 4` would execute the command line:

.. code-block:: bash

    scan-build --keep-going make --directory build --jobs 4
