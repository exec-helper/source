.. _exec-helper-plugins-clang-tidy:

Clang-tidy plugin
*****************

Description
===========
The clang-tidy plugin is used for executing the clang-tidy static code analysis tool.

Mandatory settings
==================
There are no mandatory settings for the clang-tidy plugin.

Optional settings
=================
The configuration of the clang-tidy plugin may contain the following settings:

.. program:: exec-helper-plugins-clang-tidy

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: sources

    A list of sources that must be checked by the clang-tidy plugin. The sources may contain wildcards.

.. describe:: checks

    A list of checks that should be enabled or disabled. Enabling or disabling checks is done the same way as they are enabled on the clang-tidy command line. Default: no checks will be enabled or disabled on the command line, meaning the default checks enabled by clang will be checked.

.. describe:: warning-as-errors

    Threat warnings as errors.
    The value associated with this key is either:

    * A list of checks, defining which warnings will be threated as errors. See **checks** for the format.
    * The single keyword *all*: means that all enabled checks will be threated as errors.

    **Note**: This options is only supported if the clang-tidy binary supports the :code:`-warnings-as-error=<string>` option.
 
Example
=======
Configuration
-------------

.. literalinclude:: ../examples/clang-tidy.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following files need to be created in the *src* directory:

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

See also
========
See :ref:`exec-helper` (1) for information about the usage of :program:`exec-helper`.

See :ref:`exec-helper-config` (5) for information about the configuration file format.

See :ref:`exec-helper-plugins` (5) for information about the configuration file format.
