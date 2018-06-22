.. _INSTALL:

Installation instructions
*************************
Installing from package
=======================
Arch Linux based distributions
------------------------------
Arch linux users can use the `exec-helper (AUR) package <https://aur.archlinux.org/packages/exec-helper>`_ or checkout the `exec-helper-package <https://github.com/bverhagen/exec-helper-package>`_ project for building the package from source.

Debian based distributions
--------------------------
Add the `PPA on Launchpad <https://launchpad.net/~bverhagen/+archive/ubuntu/exec-helper>`_ to your sources or checkout the `exec-helper-package <https://github.com/bverhagen/exec-helper-package>`_ project for building the package from source.

Other distributions
-------------------
Checkout the `Building from source`_ section.

Building from source
====================
Requirements
------------
Build tools
~~~~~~~~~~~
* A C++ 17 compatible compiler. Tested with: *g++*, *clang++* and MSVC 2017 (14.1)
* cmake (3.15 or newer)
* make or ninja
* make for the quick install
* Sphinx for generating man-pages and general documentation
* Doxygen (1.8.15 or newer) for building API documentation (optional)
* gitchangelog for building the changelog (optional)

Build dependencies
~~~~~~~~~~~~~~~~~~
* POSIX compliant operating system
* `boost-program-options <https://github.com/boostorg/program_options>`_ (1.64 or newer) development files
* `boost-log <https://github.com/boostorg/log>`_ (1.64 or newer) development files
* `yaml-cpp <https://github.com/jbeder/yaml-cpp>`_ (0.5.3 or newer) development files (optional, will be downloaded and compiled in statically if missing)
* `Microsoft gsl <https://github.com/Microsoft/GSL>`_ development files (optional, will be downloaded and compiled in statically if missing)
* `luaJIT <https://luajit.org/>`_ development files (optional, will be downloaded an compiled in statically if missing)

Quick installation
------------------

.. code-block:: sh

    $ make
    $ sudo make install

Use

.. code-block:: sh

    $ make help

for an overview of the available quick installation targets and for an overview of the (very limited) available configuration options.

Advanced installation
---------------------
CMake is the build system. The *Makefile* mentioned in the quick installation is a simple wrapper around a more complex - and more configurable - CMake invocation.

It has the following project-specific configuration options:

.. describe:: USE_SYSTEM_YAML_CPP

    Use the `yaml-cpp <https://github.com/jbeder/yaml-cpp>`_ library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: *auto*

.. describe:: USE_SYSTEM_LUAJIT

    Use the `luaJIT <https://luajit.org/>`_ library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: *auto*

.. describe:: USE_SYSTEM_GSL

    Use the `Microsoft gsl <https://github.com/Microsoft/GSL>`_ library installed on the system. If switched off, the library will be automaically installed locally during the build. Default: *auto*

.. describe:: POSITION_INDEPENDENT_CODE

    Build using `position independent code <https://cmake.org/cmake/help/latest/variable/CMAKE_POSITION_INDEPENDENT_CODE.html>`_. Default: *ON*

.. describe:: ENABLE_TESTING

    Enable building the testing infrastructure. Default: *ON*

.. describe:: BUILD_MAN_DOCUMENTATION

    Generate the man-pages for this project

.. describe:: BUILD_HTML_DOCUMENTATION

    Generate the HTML documentation for this project

.. describe:: BUILD_XML_DOCUMENTATION

    Generate the XML documentation for this project

Build tests
-----------
Testing is enabled by setting the CMake configuration option *ENABLE_TESTING* to *ON*.

The tests require, in addition to all dependencies above, the following dependencies:

* `Catch2 <https://github.com/catchorg/Catch2>`_ unittest framework development files (optional, for building the tests)
* `Rapidcheck <https://github.com/emil-e/rapidcheck>`_ property based unittest framework development files (optional, for building the tests)

Testing related configuration options:

.. describe:: ENABLE_WERROR

    Enable warning as error during compilation (only supported for *GCC* and *clang*)

.. describe:: LIMITED_OPTIMIZATION

    Build with limited optimization (typically -O1, only supported for *GCC* and *clang*). This is typically used for running tools like valgrind.

.. describe:: TERMINATE_ON_ASSERT_FAILURE

    Explicitly terminate when an assert fires.

.. describe:: USE_SYSTEM_CATCH

    Use the `Catch2 <https://github.com/catchorg/Catch2>`_ library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: *auto*

.. describe:: USE_SYSTEM_RAPIDCHECK

    Use the `Rapidcheck <https://github.com/emil-e/rapidcheck>`_ library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: *auto*

Cross compilation
=================
Exec-helper supports both native and cross compilation (including building with a custom sysroot) builds. Cross compilation requires invoking cmake directly and appending **-DCMAKE_TOOLCHAIN_FILE=<toolchain-file>** to the cmake initialization command. Check the *toolchain.cmake.in* file for a template on setting up the toolchain file for cross compilation and the *Makefile* for a template of the cmake initialization command.


