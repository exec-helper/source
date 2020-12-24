.. _exec-helper-plugins-zsh:

Zsh plugin
**********

Description
===========
The zsh plugin is used for executing commands in the `zsh` shell, rather than executing the command right away. This is very useful for executing command lines that need special shell characters like :code:`&&`, :code:`|`, :code:`;`, :code:`>`.

Mandatory settings
==================

Mandatory settings for all modes
--------------------------------

.. program:: exec-helper-plugins-zsh

.. describe:: command

    Command to execute in the shell, as a string. See the :code:`-c` option of :code:`zsh` for more information.

Optional settings
=================
The configuration of the zsh plugin may contain the following additional settings:

.. program:: exec-helper-plugins-zsh

.. include:: command-line.rst

.. include:: patterns.rst

.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

Example
=======

Configuration
-------------

.. literalinclude:: ../examples/zsh.example
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
