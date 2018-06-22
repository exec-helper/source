.. _exec-helper-config:

Configuration
*************

.. toctree::
    :maxdepth: 0
    :hidden:
    :glob:

    exec-helper-config-*

Description
===========
Exec-helper configuration files are written in the YAML 1.2 specification. 

Mandatory keys
==============
A valid configuration file must contain at least the following keys on the root level of the configuration file:

.. program:: exec-helper-config

.. describe:: commands

The commands that are configured in the configuration file. It will either contain a list of commands or a list of the commands as keys with an explanation of the command as a value. These formats can not be used interchangeably.

.. describe:: <command-keys>

For every command defined under the *commands* key, the configuration must define this command as a key in the root of the configuration file. The value of the key must either be a registered plugin or another command.

.. describe:: <plugin-keys>

For at least every plugin that is used by a *command* key, configure the specifics of the plugin (if applicable).

Optional keys
=============
Optionally the configuration file contains the following keys on the root level of the configuration file:

.. describe:: patterns

    Patterns are parts of the configuration that will be replaced by its value when evaluated by exec-helper. The *patterns* keyword describes a list of patterns identified by their key. See the @ref exec-helper-config-patterns for more information about how to define a pattern.

.. describe:: additional-search-paths

    An ordered list of additional search paths to use when searching for plugins. The search paths can be absolute or relative w.r.t. the parent path of the *settings file* in which these paths are defined.

    Defining search paths is useful for extending exec-helper with your own custom plugins or for overwriting or extending the functionality in the provided plugins. See [exec-helper-custom-plugins](@ref exec-helper-custom-plugins)(5) for more information on writing a custom plugin.

    The paths defined in this list take precedence over the system search paths for modules with the same name. A higher position in this list implicates higher precedence.

Working directory
=================
Configured commands are executed from the so-called *working directory*. Executing commands in a different working directory will not affect your current working directory (e.g. when executing from a shell). Each separately configured command can be executed in a separate working directory.

The *working directory* is the directory that is associated with the first of the following lines whose requirement is met:
1. The *working-dir* configuration setting is configured for the specific command. The value of the *working-dir* configuration key can be an absolute path to the working directory or a relative one w.r.t. the directory of the considered configuration file. If the command should be executed in the actual working directory, use *<working-dir>* as the value in the configuration file.
2. The directory of the considered configuration file.

Paths
=====
All relative paths in the configuration should be *relative to the directory in which the configuration resides*. While relative paths are convenient for users as they can freely choose the root directory of an application, some applications require an absolute path. In such case, use the *${PWD}*  environment variable (both POSIX and non-POSIX systems) to convert a relative path in your configuration into an absolute path for calling these particular applications.

Example configuration
=====================

.. literalinclude:: ../examples/exec-helper-config.example
    :language: yaml

See also
========

See :ref:`exec-helper-config-patterns` (5) for more information on defining and using patterns.

See :ref:`exec-helper-config-environment` (5) for more information on configuring execution environments.

See :ref:`exec-helper` (1) for information about the usage of exec-helper.

See :ref:`exec-helper-plugins` (5) for the available plugins and their configuration options.

See :ref:`exec-helper-custom-plugins` (5) for the available plugins and their configuration options.
