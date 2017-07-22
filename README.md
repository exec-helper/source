# exec-helper
Or How To Get Your Coffee In Peace. That is al you ever wanted, no?

## Continuous integration
[![build status](https://gitlab.com/bverhagen/exec-helper/badges/master/build.svg)](https://gitlab.com/bverhagen/exec-helper/commits/master)
[![coverage report](https://gitlab.com/bverhagen/exec-helper/badges/master/coverage.svg)](https://gitlab.com/bverhagen/exec-helper/commits/master)

## Why
Exec-helper is a meta-wrapper for executing repeatable tasks.

## Building a package
Checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building exec-helper as a package, if your system is supported.

## Building from source
### Requirements
- A POSIX compliant operating system
- A C++ 11 compatible compiler
- boost-program-options development files
- yaml-cpp development files (optional, will be downloaded and compiled in statically if missing)
- catch unittest framework development files for building the tests (optional)
- Doxygen for building the man-pages and/or HTML documentation (optional)
- gitchangelog for building the changelog (optional)

### Quick installation
```sh
$ make
# make install
```

Note: uses the CC and CXX environment variables for determining the compiler or the system's default compiler otherwise. Check the 'less quick install'-section for using other compilers.

## Less quick install
Check the [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html).

## Documentation
See [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html) for the latest documentation.

## Configuration
Check the [Documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html) for more information about configuring exec-helper.
