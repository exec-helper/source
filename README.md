# exec-helper
A helper for executing development related stuff more efficiently. Exec-helper aims for configurability.

## Continuous integration
[![build status](https://gitlab.com/bverhagen/exec-helper/badges/master/build.svg)](https://gitlab.com/bverhagen/exec-helper/commits/master)

Check the mirrored [Gitlab project](https://gitlab.com/bverhagen/exec-helper) for more information about the build and test status of the project.

## Why
The development workflow contains several repeatable actions (initialize a workspace, build in several configurations, run and profile the built files...). Exec-helper wants to make life easier for the developer by bundling all these actions into one reusable command line. It aims to be a wrapper on top of several (of the supported) systems.

## Requirements
- Python (both 2.7.x and 3 are supported)

## Configuration
Exec-helper can be configured so your often used options can be set by default. Command line parameters will always override this configuration. When a file named *.exec-helper_profiles* exists in the working directory, it is considered to be the configuration settings file, unless this is overriden by the command line argument. See the *--help* option of Exec-helper for more information. The configuration is defined using the JSON format. Currently the following settings are supported:

| Setting           | Explanation       | JSON format       |
| ----------------- | ----------------- | ----------------- |
| profile map       | A mapping of the existing profiles in the project | JSON array of profiles |
| profile           | Defines a run target of the project | { "id" : \<id\>, "suffix" : \<suffix\>, "targetDirectory": \<target directory\> } |
| id                | The id of the profile | "id" : JSON string |
| suffix            | The suffix applied after the target in the output binaries | "suffix" : JSON string |
| targetDirectory   | The target directory in which to find the output binary | "targetDirectory" : JSON string |
| targets           | List of targets defined in the project | "targets" : JSON array of strings |
| compilers         | List of compilers to build for the project | "compilers" : JSON array of strings |
| modes             | List of modes to build for. Currently restricted to *debug* and *release* | "modes" : JSON array of strings |
| verbosity         | Whether we would execute Exec-helper in verbose mode | "verbosity" : "yes" or "no" to enable/disable |
| Single threaded build | Build the project single threaded (multi-threaded by default) | "single-threaded" : "yes" or "no" to enable/disable |
| Build system      | The build system to use with the *build* command | "build-system" : JSON string |
| Analyze methods   | The analyze methods to use with the *analyze* command | "analyze-system" : JSON array of strings |
| Toolchain path    | Path to the toolchain to use with the *build* command | "toolchain-path" : JSON string |
| Show stuff        | Enable/disable interactive results | "show-stuff" : "yes" or "no" to enable/disable |
| Build system      | The build system to use for the backend. See below for a list of the supported ones | "build-system" : "<buildsystem>" |
| Root build dir    | The root directory to use for a certain build. The following patterns can be used in the string and will be replaced by its respective value: {COMPILER} for the compiler name and {MODE} for the build mode | "root-build-dir" : "build/{COMPILER}/{MODE} |

An example of the configuration file can be found in *example/.exec-helper_profiles*.

## Usage
```
$ ./exec [commands] <options>
```

You may want to add a symbolic link to *exec* or *exec.py* in the exec-helper directory that is contained within your path:
```
# ln -fs <path to exec-helper>/exec.py /usr/bin/exec-helper
```
Check the *--help* option for more information.

## Currently supported systems
Note: it is currently not possible to add your own custom systems

### Build modes
- release
- debug

### Build systems
- scons
- make (only for one specific configuration, no support for a generic make system yet)
- fake (for testing purposes)

### Compilers
- gcc
- clang

### Analyze methods
- clang-analyzer
- cppcheck
- valgrind
- cpd
- simian
- cpplint

### Profile methods
- perf
- callgrind
