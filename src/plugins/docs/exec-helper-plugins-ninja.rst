.. _exec-helper-plugins-ninja:

Ninja plugin
************
Description
===========
The ninja plugin is used for executing Makefiles.

Mandatory settings
==================
There are no mandatory settings for this plugin.

Optional settings
=================
The configuration of the ninja plugin may contain the following settings:

.. program:: exec-helper-plugins-ninja
 
.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: build-dir

    The path to the build directory. This is either an absolute path are a path relative to the location of this file. Default: :code:`.` (the current working directory).

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/ninja.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*ninja.build*:

.. literalinclude:: ../examples/build.ninja
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
