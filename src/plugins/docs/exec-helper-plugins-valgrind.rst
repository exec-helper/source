.. _exec-helper-plugins-valgrind:

Valgrind plugin
***************
Description
===========
The valgrind plugin is used for executing code coverage analysis using valgrind.

Mandatory settings
==================
The configuration of the valgrind plugin must contain the following settings:

.. program:: exec-helper-plugins-valgrind

.. describe:: run-command

    The exec-helper command or plugin to use for running the binaries which need to be analyzed.

Optional settings
=================
The configuration of the valgrind plugin may contain the following settings:

.. program:: exec-helper-plugins-valgrind

.. include:: patterns.rst

.. include:: command-line.rst

.. describe:: tool

    The valgrind tool to use. Default: the :code:`tool` is omitted.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/valgrind.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*Makefile*:

.. literalinclude:: ../examples/Makefile
    :language: none

*hello.cpp:*

.. literalinclude:: ../examples/src/hello.cpp
    :language: cpp

*world.cpp:*

.. literalinclude:: ../examples/src/world.cpp
    :language: cpp

Usage
-----
Save the example to an :program:`exec-helper` configuration file and execute in the same directory:

.. code-block:: bash

    eh example

.. include:: see-also.rst
