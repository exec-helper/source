.. _exec-helper-plugins-clang-static-analyzer:

Clang-static-analyzer plugin
********************************
Description
===========
The clang-static-analyzer plugin is used for executing the clang-static-analyzer static code analysis tool.

Mandatory settings
==================
The configuration of the clang-static-analyzer plugin must contain the follwing settings:

.. program:: exec-helper-plugins-clang-static-analyzer

.. describe:: build-command

    The :program:`exec-helper` build target command or plugin to execute for the analysis.

Optional settings
=================
The configuration of the clang-static-analyzer plugin may contain the following settings:
 
.. program:: exec-helper-plugins-clang-static-analyzer

.. include:: patterns.rst

.. include:: command-line.rst

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/clang-static-analyzer.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*Makefile*:

.. literalinclude:: ../examples/Makefile
    :language: make

*src/hello.cpp:*

.. literalinclude:: ../examples/src/hello.cpp
    :language: cpp

*src/world.cpp:*

.. literalinclude:: ../examples/src/world.cpp
    :language: cpp

Usage
-----
Save the example to an :program:`exec-helper` configuration file and execute in the same directory:

.. code-block:: bash

    eh example

See also
--------
See :ref:`exec-helper` (1) for information about the usage of :program:`exec-helper`.

See :ref:`exec-helper-config` (5) for information about the configuration file format.

See :ref:`exec-helper-plugins` (5) for information about the configuration file format.
