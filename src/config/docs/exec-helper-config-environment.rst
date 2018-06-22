.. _exec-helper-config-environment:

Environment
***********

Description
===========
Environment variables can be configured in the configuration file. They will only be set for the particular command(s) defined by the relevant section of the configuration.

Environment variables can *not* be set directly in a command line command. The :program:`environment` configuration key needs to be used for this. See section 'environment'.

Environment
===========
The :program:`environment` keyword can be set for every plugin that supports the env configuration setting. Check the documentation on a specific plugin to check whether the plugin supports this configuration setting.

The :program:`environment` keyword must contain a *map* of key-value pairs, where the key is the name of the :program:`environment` variable and the value is the value associated with the specified :program:`environment` variable. :ref:`exec-helper-config-patterns` can be used for the :program:`environment` these variable values too.

**Note**: The *PWD* environment variable, following POSIX convention, is set by the application to the working directory of the task. Therefore, its value cannot be overriden in the configuration.

Example configuration
=====================

.. literalinclude:: ../examples/exec-helper-config.example
    :language: yaml

See also
========

See :ref:`exec-helper-config` (5) for information about the configuration file.
