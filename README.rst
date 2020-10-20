.. _mainpage:

Exec-helper
***********
Or How To Get Your Coffee In Peace.

|documentation| |nbsp| |build_badge| |nbsp| |build_windows_badge| |nbsp| |coverage_badge| |nbsp| |code_quality_badge|

What
====
Exec-helper is a meta-wrapper for executing tasks on the command line.

Why
===
Exec-helper improves the main bottleneck in your development workflow: you.

It does this by:

* Reducing the number of keystrokes required to execute the same command over and over again
* Chaining multiple commands

All without sacrificing (much) flexibility or repeating useless work.

If this, together with *getting coffee in peace* is not a sufficient rationale for you, the main advantages of exec-helper over (simple) scripts or plain command line commands are:

* Easy permutation of multiple execution parameters (so-called *patterns* in exec-helper).
* Easy selection of a subset of execution parameters.
* Improved DRY: execution parameters are only changed on one spot, in stead of everywhere in your command line.
* Technology-agnostic approach: e.g. running the `exec-helper build` can build a C++ project in one directory structure and a JAVA project in another.
* Enables a self-documented workflow.
* Out of the box support for multi-valued options and default values.
* Searches for a suitable configuration in its parent folders.
* Fast to type using the `eh` alias
* Easy to find and/or list available commands using the `--help` option.
* Easy extensible with your own, first-class citizen, plugins.
* Automatic autocompletion of commands and patterns

Simple example
==============
This is a simple illustration of the concept behind exec-helper. More extensive information and examples can be found in the *.exec-helper* configuration file for this repository and in the `documentation <http://exec-helper.readthedocs.io>`_.

Use case
--------
Build a C++ project using g++ and clang++ using cmake in a *Debug* and *RelWithDebInfo* configuration

Configuration file
------------------
Copy the following to a file named '.exec-helper'::

    commands:
        init: Initialize build
        build: Build-only + install
        build-only: Build
        install: Install

    patterns:
        COMPILER:
            default-values:
                - g++
                - clang++
            short-option: c
            long-option: compiler

        MODE:
            default-values:
                - debug
                - release
            short-option: m
            long-option: mode

    build:
        - build-only
        - install

    init:
        - command-line-command

    build-only:
        - make

    install:
        - make

    command-line-command:
        init:
            patterns:
                - COMPILER
                - MODE
            command-line: [ cmake, -H., "-Bbuild/{COMPILER}/{MODE}", "-DCMAKE_CXX_COMPILER={COMPILER}", "-DCMAKE_INSTALL_PREFIX=install/{COMPILER}/{MODE}", "-DCMAKE_BUILD_TYPE={MODE}"]

    make:
        patterns:
            - COMPILER
            - MODE
        build-dir: "build/{COMPILER}/{MODE}"
        install:
            command-line: install


Example output
--------------

.. code-block:: bash

    $ exec-helper --help
       -h [ --help ]              Produce help message
       --version                  Print the version of this binary
       -v [ --verbose ]           Set verbosity
       -j [ --jobs ] arg          Set number of jobs to use. Default: auto
       -n [ --dry-run ]           Dry run exec-helper
       -s [ --settings-file ] arg Set the settings file
       -d [ --debug ] arg         Set the log level
       -z [ --command ] arg       Commands to execute
       -c [ --compiler ] arg      Values for pattern 'compiler'
       -m [ --mode ] arg          Values for pattern 'mode'

     Configured commands:
       init                 Initialize build
       build                Build-only + install
       build-only           Build
       install              Install

    $ exec-helper init build    # Permutate all combinations of the default values
    Executing "cmake -H. -Bbuild/g++/debug -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=install/g++/debug -DCMAKE_BUILD_TYPE=debug"
    Executing "cmake -H. -Bbuild/g++/release -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=install/g++/release -DCMAKE_BUILD_TYPE=release"
    Executing "cmake -H. -Bbuild/clang++/debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=install/clang++/debug -DCMAKE_BUILD_TYPE=debug"
    Executing "cmake -H. -Bbuild/clang++/release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=install/clang++/release -DCMAKE_BUILD_TYPE=release"
    Executing "make --directory build/g++/debug --jobs 8"
    Executing "make --directory build/g++/release --jobs 8"
    Executing "make --directory build/clang++/debug --jobs 8"
    Executing "make --directory build/clang++/release --jobs 8"
    Executing "make --directory build/g++/debug --jobs 8 install"
    Executing "make --directory build/g++/release --jobs 8 install"
    Executing "make --directory build/clang++/debug --jobs 8 install"
    Executing "make --directory build/clang++/release --jobs 8 install"

    $ exec-helper build-only --compiler g++ --mode release      # Only build the g++ build in release mode
    Executing make --directory build/g++/release --jobs 8

    $ exec-helper install --compiler g++ --mode debug RelWithDebInfo    # Install a subset - even using ones not listed in the default values
    Executing make --directory build/g++/debug --jobs 8 install
    Executing make --directory build/g++/RelWithDebInfo --jobs 8 install

Installation
============
See `INSTALL <https://exec-helper.readthedocs.io/en/master/INSTALL.html>`_ for more information on:

* Using one of the available packages or installers
* (Cross-)build from source

Documentation
=============
See `documentation <http://exec-helper.readthedocs.io>`_ for the latest documentation.

Usage
-----
see `exec-helper <https://exec-helper.readthedocs.io/en/master/src/applications/exec-helper.html>`_ for usage information.

Configuration
-------------
See `exec-helper-config <https://exec-helper.readthedocs.io/en/master/src/config/docs/exec-helper-config.html>`_ for information on the configuration file format.

Available plugins
-----------------
See `exec-helper-plugins <https://exec-helper.readthedocs.io/en/master/src/plugins/docs/exec-helper-plugins.html>`_ for a list of all available plugins.

Writing custom plugins
----------------------
See `exec-helper-custom-plugins <https://exec-helper.readthedocs.io/en/master/src/plugins/docs/exec-helper-custom-plugins.html>`_ for a guide on writing your own plugins.

Code quality
============
The source code of this project is continuously analyzed by multiple tools in an attempt to catch and fix issues and bugs as quickly as possible. Released versions should have passed the analysis from the following tools:

* `AddressSanitizer <https://clang.llvm.org/docs/AddressSanitizer.html>`_ (ASan)
* `clang-format <https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html>`_
* `clang-static-analyzer <https://clang-analyzer.llvm.org>`_
* `clang-tidy <http://clang.llvm.org/extra/clang-tidy>`_
* `cppcheck <http://cppcheck.sourceforge.net>`_
* License Scanning (by Gitlab)
* `pmd <https://pmd.github.io>`_ (cpd)
* `Static Application Security Testing <https://docs.gitlab.com/ee/user/application_security/sast>`_ (SAST by Gitlab)
* `Valgrind <http://valgrind.org>`_ (memcheck)
* `UndefinedBehaviorSanitizer <https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html>`_ (UBSan)

Check the *.exec-helper* file for detailed information about how these analysis methods are configured and used. The analysis tools can be executed locally using exec-helper with this project.

.. |build_badge| image:: https://gitlab.com/bverhagen/exec-helper/badges/master/pipeline.svg
                 :alt: Commits
                 :target: https://gitlab.com/bverhagen/exec-helper/commits/master
.. |build_windows_badge| image:: https://ci.appveyor.com/api/projects/status/6hj664td3tjx9cs5/branch/master?svg=true
                         :alt: Windows build
.. |coverage_badge| image:: https://app.codacy.com/project/badge/Coverage/6b2efb802a97469ea6d010b12fd14584
                    :alt: Test coverage report
                    :target: https://app.codacy.com/gl/exec-helper/source/dashboard
.. |code_quality_badge| image:: https://api.codacy.com/project/badge/Grade/98d9b8174f0d4a8ba79adebda064093d
                        :alt: Code quality report
                        :target: https://app.codacy.com/gl/exec-helper/source/dashboard
.. |documentation| image:: https://readthedocs.org/projects/exec-helper/badge/?version=master
                   :target: https://exec-helper.readthedocs.io
                   :alt: Documentation Status
.. |nbsp| unicode:: 0xA0
   :trim:
