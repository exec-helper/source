.. _exec-helper-plugins-command-line-command:

Command-line-command plugin
*******************************

Description
===========
The command-line-command plugin is used for executing arbitrary command lines. This plugin can be used for constructing the command line for commands that do not have a corresponding plugin available.

Mandatory settings
==================
The configuration of the command-line-command must contain the following settings:

.. program:: exec-helper-plugins-command-line-command

.. describe:: command-line

    The command-line to execute. There are two different usages:

    * **No identification key**: Set one command line as a list of separate arguments. This form is only usable if only one line needs to be executed.
    * **With identification key**: Make a map with arbitrary keys, where each associated value is one command line, described as a list of separate arguments. This form is usable if one or more lines need to be executed. Multiple commands are executed in the order the identification keys are defined.
      
    **Note**: see the documentation of **wordexp** (3) for the limitations on what characters are not allowed in the command-line command.

Optional settings
=================
The configuration of the command-line-command plugin may contain the following settings:

.. program:: exec-helper-plugins-command-line-command

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: working-dir.rst

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/command-line-command.example
    :language: yaml

Usage
-----
Save the example to an :program:`exec-helper` configuration file and execute in the same directory:

.. code-block:: bash

    eh example

See also
========
See :ref:`exec-helper` (1) for information about the usage of :program:`exec-helper`.

See :ref:`exec-helper-config` (5) for information about the configuration file format.

See :ref:`exec-helper-plugins` (5) for information about the configuration file format.
