exec-helper   {#mainpage}
==========
Or How To Get Your Coffee In Peace.

[![build status](https://gitlab.com/bverhagen/exec-helper/badges/master/build.svg?branch=master)](https://gitlab.com/bverhagen/exec-helper/commits/master)
[![coverage report](https://gitlab.com/bverhagen/exec-helper/badges/master/coverage.svg?job=coverage)](http://bverhagen.gitlab.io/exec-helper/coverage/index.html)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/98d9b8174f0d4a8ba79adebda064093d)](https://www.codacy.com/app/bverhagen/exec-helper?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bverhagen/exec-helper&amp;utm_campaign=Badge_Grade)

# What
Exec-helper is a meta-wrapper for executing tasks on the command line.

# Why
Exec-helper improves the main bottleneck in your development workflow: you.

It does this by:

- Reducing the number of keystrokes required to execute the same command over and over again
- Chaining multiple commands

All without sacrificing (much) flexibility or repeating useless work.

If this, together with _getting coffee in peace_ is not a sufficient rationale for you, the main advantages of exec-helper over (simple) scripts or plain command line commands are:
- Easy permutation of multiple execution parameters (so-called _patterns_ in exec-helper).
- Easy selection of a subset of execution parameters.
- Improved DRY: execution parameters are only changed on one spot, in stead of everywhere in your command line.
- Technology-agnostic approach: e.g. running the `exec-helper build` can build a C++ project in one directory structure and a JAVA project in another.
- Enables a self-documented workflow.
- Out of the box support for multi-valued options and default values.
- Searches for a suitable configuration in its parent folders.
- Fast to type using the `eh` alias
- Easy to find and/or list available commands using the `--help` option.

# Simple example
This is a simple illustration of the concept behind exec-helper. More extensive information and examples can be found in the _.exec-helper_ configuration file for this repository and in the [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html).

## Use case
Build a C++ project using g++ and clang++ using cmake in a _Debug_ and _RelWithDebInfo_ configuration

## Configuration file
Copy the following to a file named '.exec-helper':

```yaml
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

```

## Example output

```c
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
```

# Installation
See [Installation instructions](@ref INSTALL) for more information on:
- Using one of the available packages or installers
- (Cross-)build from source

# Documentation
See [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html) for the latest documentation.

## Usage
see [Exec Helper](@ref exec-helper) for usage information.

## Configuration
See [Exec Helper Config](@ref exec-helper-config) for information on the configuration file format.

## Available Plugins
See [Exec Helper Plugins](@ref exec-helper-plugins) for a list of all available plugins.

# Code quality
The source code of this project is continuously analyzed by multiple tools in an attempt to catch and fix issues and bugs as quickly as possible. Released versions should have passed the analysis from the following tools:
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) (ASan)
- [clang-format](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- [clang-static-analyzer](https://clang-analyzer.llvm.org)
- [clang-tidy](http://clang.llvm.org/extra/clang-tidy)
- [coverity](https://scan.coverity.com/projects/exec-helper)
- [cppcheck](http://cppcheck.sourceforge.net)
- [pmd](https://pmd.github.io) (cpd)
- [valgrind](http://valgrind.org) (memcheck)
- [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (UBSan)

Check the _.exec-helper_ file for detailed information about how these analysis methods are configured and used. The analysis tools can be executed locally using exec-helper with this project.

# Project development documentation
## Feature documentation
See [feature documentation](@ref feature-documentation) for a list of documented features.

## Scenario documentation
[Features](@ref feature-documentation) are mapped onto scenario's. Scenario's should map on one or more integration test cases.
