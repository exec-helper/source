.. _exec-helper-plugins-scons:

Scons plugin
************
Description
===========
The scons plugin is used for executing scons.

Mandatory settings
==================
There are no mandatory settings for this plugin.

Optional settings
=================

.. program:: exec-helper-plugins-scons

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

.. literalinclude:: ../examples/scons.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*SConstruct*:

.. literalinclude:: ../examples/SConstruct
    :language: none

*SConscript*:

.. literalinclude:: ../examples/src/SConscript
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
