.. _exec-helper-plugins-sh:

Sh plugin
*********

Description
===========
The sh plugin is used for executing commands in the `sh` shell, rather than executing the command right away. This is very useful for executing command lines that need special shell characters like :code:`&&`, :code:`|`, :code:`;`, :code:`>`.

Mandatory settings
==================

Mandatory settings for all modes
--------------------------------

.. program:: exec-helper-plugins-sh

.. describe:: command

    Command to execute in the shell, as a string. See the :code:`-c` option of :code:`sh` for more information.

Optional settings
=================
The configuration of the sh plugin may contain the following additional settings:

.. program:: exec-helper-plugins-sh

.. include:: command-line.rst

.. include:: patterns.rst

.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

Example
=======

Configuration
-------------

.. literalinclude:: ../examples/sh.example
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
