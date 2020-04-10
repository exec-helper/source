Installation instructions   {#INSTALL}
=========================
# Installing from package
## Arch Linux based distributions
Arch linux users can use the [exec-helper](https://aur.archlinux.org/packages/exec-helper) (AUR) package or checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building the package from source.

## Debian based distributions
Add the [PPA on Launchpad](https://launchpad.net/~bverhagen/+archive/ubuntu/exec-helper) to your sources or checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building the package from source.

## Other distributions
Checkout the [Building from source](#building-from-source) section.

# Building from source
## Requirements
### Host build dependencies
- cmake (3.15 or newer)
- make or ninja
- make for the quick install
- Doxygen (1.8.15 or newer) for building the man-pages and/or HTML documentation (optional)
- gitchangelog for building the changelog (optional)

### Target build dependencies
- POSIX compliant operating system
- A C++ 17 compatible compiler. Tested with: _g++_, _clang++_ and MSVC 2017 (14.1)
- [boost-program-options](https://github.com/boostorg/program_options) (1.64 or newer) development files
- [boost-log](https://github.com/boostorg/log) (1.64 or newer) development files
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) (0.5.3 or newer) development files (optional, will be downloaded and compiled in statically if missing)
- [Microsoft gsl](https://github.com/Microsoft/GSL) development files (optional, will be downloaded and compiled in statically if missing)
- [catch2](https://github.com/catchorg/Catch2) unittest framework development files for building the tests (optional, for building the tests)

## Quick installation

```sh
$ make
# make install
```

Use
```sh
$ make help
```
for an overview of the available quick installation targets and for an overview of the (very limited) available configuration options.

## Advanced installation
CMake is the build system. The _Makefile_ mentioned in the quick installation is a simple wrapper around a more complex - and more configurable - CMake invocation.

It has the following project-specific configuration options:
- **USE\_SYSTEM\_YAML\_CPP**: Use the [yaml-cpp](https://github.com/jbeder/yaml-cpp) library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: _auto_
- **USE\_SYSTEM\_LUAJIT**: Use the [luaJIT](https://luajit.org/) library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: _auto_
- **USE\_SYSTEM\_LUAWRAPPER**: Use the [luawrapper](https://github.com/ahupowerdns/luawrapper/) library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: _auto_
- **POSITION\_INDEPENDENT\_CODE**: Build using [position independent code](https://cmake.org/cmake/help/latest/variable/CMAKE_POSITION_INDEPENDENT_CODE.html). Default: _ON_
- **ENABLE\_TESTING**: Enable building the testing infrastructure. Default: _ON_
- **BUILD\_MAN\_DOCUMENTATION**: Generate the man-pages for this project
- **BUILD\_HTML\_DOCUMENTATION**: Generate the HTML documentation for this project
- **BUILD\_XML\_DOCUMENTATION**: Generate the XML documentation for this project

## Build tests
Testing is enabled by setting the CMake configuration option _ENABLE\_TESTING_ to _ON_.

The tests require, in addition to all dependencies above, the following dependencies:
- Catch2
- Rapidcheck

Testing related configuration options:
- **ENABLE\_WERROR**: Enable warning as error during compilation (only supported for _GCC_ and _clang_)
- **LIMITED\_OPTIMIZATION**: Build with limited optimization (typically -O1, only supported for _GCC_ and _clang_). This is typically used for running tools like valgrind.
- **TERMINATE\_ON\_ASSERT\_FAILURE**: Explicitly terminate when an assert fires.
- **USE\_SYSTEM\_CATCH**: Use the [Catch2](https://github.com/catchorg/Catch2) library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: _auto_
- **USE\_SYSTEM\_RAPIDCHECK**: Use the [Rapidcheck](https://github.com/emil-e/rapidcheck) library installed on the system. If switched off, the library will be automatically installed locally during the build. Default: _auto_

# Cross compilation
Exec-helper supports both native and cross compilation (including building with a custom sysroot) builds. Cross compilation requires invoking cmake directly and appending *-DCMAKE_TOOLCHAIN_FILE=\<toolchain-file\>* to the cmake initialization command. Check the _toolchain.cmake.in_ file for a template on setting up the toolchain file for cross compilation and the _Makefile_ for a template of the cmake initialization command.


