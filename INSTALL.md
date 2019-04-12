Installation instructions   {#INSTALL}
=========================
# Installing from package
## Arch Linux based distributions
Arch linux users can use the [exec-helper-git](https://aur.archlinux.org/packages/exec-helper-git) (AUR) package or checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building the package from source.

## Debian based distributions
Checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building exec-helper as a package, if your system is supported.

## Other distributions
Checkout the [Building from source](#building-from-source) section.

# Building from source
## Requirements
### Host build dependencies
- cmake
- make or ninja
- make for the quick install
- Doxygen for building the man-pages and/or HTML documentation (optional)
- gitchangelog for building the changelog (optional)

### Target build dependencies
- POSIX compliant operating system
- A C++ 17 compatible compiler. Tested with: _g++_ and _clang++_
- [boost-program-options](https://github.com/boostorg/program_options) development files
- [boost-log](https://github.com/boostorg/log) development files
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) development files (optional, will be downloaded and compiled in statically if missing)
- [Microsoft gsl](https://github.com/Microsoft/GSL) development files (optional, will be downloaded and compiled in statically if missing)
- [catch](https://github.com/philsquared/Catch) unittest framework development files for building the tests (optional, for building the tests)

## Quick installation

```sh
$ make
# make install
```

Use
```sh
$ make help
```
for an overview of the available quick installation targets and for an overview of the (very limited) available configuration options. More configuration options are available by following the 'advanced install' section.

# Cross compilation
Exec-helper supports both native and cross compilation (including building with a custom sysroot) builds. Cross compilation requires invoking cmake directly and appending *-DCMAKE_TOOLCHAIN_FILE=\<toolchain-file\>* to the cmake initialization command. Check the _toolchain.cmake.in_ file for a template on setting up the toolchain file for cross compilation and the _Makefile_ for a template of the cmake initialization command.


