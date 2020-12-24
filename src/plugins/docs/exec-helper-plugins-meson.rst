.. _exec-helper-plugins-meson:

Meson plugin
************
Description
===========
The meson plugin is used for setting up, compiling, installing and testing software using the Meson build generator system.

Mandatory settings
==================

Mandatory settings for all modes
--------------------------------

.. describe:: mode

    Set the mode of the Meson call for the specific command. Default: *setup*.

    Supported modes are:

    * **setup**: For setting up the build directory based on the Meson configuration in the source. This is often callend the *configure* or *build init* step.
    * **compile**: Compiles (or builds) the generated project
    * **test**: Run the configured test suite using Meson
    * **install**: Install the generated project

Optional settings
=================
The configuration of the meson plugin may contain the following additional settings:

Settings for all modes
----------------------

.. program:: exec-helper-plugins-meson

.. include:: patterns.rst

.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: build-dir

    The path to the build directory. This is either an absolute path are a path relative to the location of this file. Default: *.* (the directory of the :program:`exec-helper` configuration).

Additional settings for the *setup* mode
----------------------------------------

.. program:: exec-helper-plugins-meson

.. describe:: source-dir

    The directory containing the root :code:`meson.build` file of the sources. Default: *.* (the directory of the :program:`exec-helper` configuration).

.. describe:: build-type

    Set the Meson build type explicitly. See the :code:`--buildtype` parameter of :code:`meson setup` for more information.

.. describe:: cross-file

    Set the Meson cross-file. See the :code:`--cross-file` parameter of :code:`meson setup` for more information.

.. describe:: prefix

    Set the Meson installation prefix. See the :code:`--prefix` parameter of :code:`meson setup` for more information.

.. describe:: options

    A map of the options to set for setting up the build. See the :code:`-D` parameter of :code`meson setup` for more information.

Additional settings for the *compile* mode
-------------------------------------------

.. program:: exec-helper-plugins-meson

.. describe:: jobs

    Fix the number of jobs to use. Default: *auto* or the number of jobs set on the :program:`exec-helper` invocation.

Additional settings for the *test* mode
---------------------------------------

.. program:: exec-helper-plugins-meson

.. describe:: suites

    Set the test suites to run. By default, this parameter is omitted.

.. describe:: targets

    Set the targets to run. By default, this parameter is omitted.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/meson.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*meson.build*:

.. literalinclude:: ../examples/meson.build
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
