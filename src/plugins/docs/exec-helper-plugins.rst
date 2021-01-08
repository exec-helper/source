.. _exec-helper-plugins:

Plugins
*******

.. toctree::
    :maxdepth: 0
    :hidden:
    :glob:

    exec-helper-custom-plugins
    exec-helper-plugins-*

Description
===========
This document describes the list of :program:`plugins` that can be used in the associated :program:`exec-helper` binaries.

General plugins
===============

.. program:: plugins

.. describe:: command-line-command

    The command-line-command plugin is used for executing arbitrary command line commands.
    See :ref:`exec-helper-plugins-command-line-command` (5).

.. describe:: execute-plugin

    The execute plugin is used for executing other commands and/or targets configured in the :program:`exec-helper` configuration file. See :ref:`exec-helper-plugins-execute-plugin` (5).

.. describe:: selector

    The selector plugin is used for selecting certain configuration paths based on the value of a pattern. See :ref:`exec-helper-plugins-selector` (5).

.. describe:: docker

    The docker plugin is used for running commands inside a Docker container. See :ref:`exec-helper-plugins-docker` (5).

Build plugins
=============

.. program:: plugins

.. describe:: bootstrap

    The bootstrap plugin is used for calling bootstrap scripts, typically used as a step in a build chain. See :ref:`exec-helper-plugins-bootstrap` (5).

.. describe:: make

    The make plugin is used for running the make build system. See :ref:`exec-helper-plugins-make` (5).

.. describe:: scons

    The scons plugin is used for running the scons build system. See :ref:`exec-helper-plugins-scons` (5).

.. describe:: cmake

    The cmake plugin is used for running the CMake build system. See :ref:`exec-helper-plugins-cmake` (5).

.. describe:: meson

    The meson plugin is used for running the CMake build system. See :ref:`exec-helper-plugins-meson` (5).

Analysis plugins
================

.. program:: plugins

.. describe:: clang-static-analyzer

    The clang-static-analyzer plugin is used for applying the clang static analyzer tool on source code files. See :ref:`exec-helper-plugins-clang-static-analyzer` (5).

.. describe:: clang-tidy

    The clang-tidy plugin is used for applying the clang tidy tool on source code files. See :ref:`exec-helper-plugins-clang-tidy` (5).

.. describe:: cppcheck

    The cppcheck plugin is used for applying cppcheck on source code files. See :ref:`exec-helper-plugins-cppcheck` (5).

.. describe:: lcov

    The lcov plugin is used for applying the lcov code coverage analysis tool. See :ref:`exec-helper-plugins-lcov` (5).

.. describe:: pmd

    The pmd plugin is used for applying pmd analysis on source code files. See :ref:`exec-helper-plugins-pmd` (5).

.. describe:: valgrind

    The valgrind plugin is used for applying valgrind analysis. See :ref:`exec-helper-plugins-valgrind` (5).

Custom plugins
==============
You can write your own plugins and integrate them with :program:`exec-helper`. These plugins are first-class citizens: you can write plugins that overwrite the system plugins themselves. See :ref:`exec-helper-custom-plugins` (5) for more information on writing your own plugins.

.. include:: see-also.rst
