.. _exec-helper-plugins-cppcheck:

Cppcheck plugin
***************
Description
===========
The cppcheck plugin is used for executing the cppcheck static code analysis tool.

Mandatory settings
==================
There are no mandatory settings for the cppcheck plugin.

Optional settings
=================
The configuration of the cppcheck plugin may contain the following settings:

.. program:: exec-helper-plugins-cppcheck

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: enable-checks

    A list of checks that should be enabled or disabled. Check the documentation of cppcheck for a list of all the available checks. Default: *all*.

.. describe:: src-dir

    The base directory containing all the files to check. Default: :code:`.` (the current working directory).

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/cppcheck.example
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
