.. _exec-helper-plugins-cmake:

CMake plugin
************
Description
===========
The cmake plugin is used for generating, building and installing software using the CMake build generator system.

Mandatory settings
==================
There are no mandatory settings for this plugin, though it is recommended to configure the *mode* setting explicitly.

Optional settings
=================
The configuration of the make plugin may contain the following settings:

Settings for all modes
----------------------

.. program:: exec-helper-plugins-cmake

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: mode

    Set the mode of the CMake call for the specific command. Default: *generate*.

    Supported modes are:

    * **Generate**: For generating a build directory based on the CMake configuration in the source. This is often callend the *configure* or *build init* step.
    * **Build**: Build the generated project
    * **Install**: Install the generated project
     
.. describe:: build-dir

    The path to the build directory. This is either an absolute path are a path relative to the location of this file. Default: *.* (the directory of the :program:`exec-helper` configuration).

Settings for the *generate* mode
--------------------------------

.. program:: exec-helper-plugins-cmake

.. describe:: source-dir

    The directory containing the root CMakeLists.txt file of the sources. Default: *.* (the directory of the :program:`exec-helper` configuration).

.. describe:: generator

    The generator to use for generating the build directory. See the CMake documentation on which generators are supported for your platform and the value(s) to explicitly set them. Default: the default one for your system and environment. See the CMake documentation on the details.

.. describe:: defines

    A map of the *build generator settings* for configuring the generator.

Settings for the *build* mode
-----------------------------

.. program:: exec-helper-plugins-cmake

.. describe:: target

    The specific CMake target to build. Default: the default target. See the CMake documentation for more details.

.. describe:: config

    The configuration for multi-configuration tools. Default: the default configuration. See the CMake documentation for more details.

Settings for the *install* mode
-------------------------------

.. program:: exec-helper-plugins-cmake

.. describe:: config

    The configuration for multi-configuration tools. Default: the default configuration. See the CMake documentation for more details.

.. describe:: prefix

    Override the configured prefix set during the *generate* mode. Default: the default installation prefix. See the CMake documentation for more details.

.. describe:: component

    Limit installation to the given component. Default: all installation targets.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/cmake.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*CMakeLists.txt*:

.. literalinclude:: ../examples/CMakeLists.txt
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

See also
========
See :ref:`exec-helper` (1) for information about the usage of :program:`exec-helper`.

See :ref:`exec-helper-config` (5) for information about the configuration file format.

See :ref:`exec-helper-plugins` (5) for information about the configuration file format.
