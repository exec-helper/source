The Docker plugin  {#exec-helper-plugins-docker}
===============
\brief An overview of the docker plugin configuration settings

# DESCRIPTION
The Docker plugin is used for running or attaching to a Docker container.

# MANDATORY SETTINGS
Mandatory settings change depending on which mode is selected. See _mode_ for more information.

# OPTIONAL SETTINGS
The configuration of the make plugin may contain the following settings:

## Settings for all modes

### mode
Set the mode of the Docker call for the specific command. Default: _exec_.

Supported modes are:

- _run_: Create a new container based on the given _image_ and runs the given command. Note: use _--rm_ as an additional command line argument to automatically clean up the created container.
- _exec_: Run the command in the given, actively running, _container_.
 
### patterns
A list of _patterns_ to apply on the command line. See [exec-helper-config-patterns](@ref exec-helper-config-patterns)(5).

### working-dir
The working directory on the host of the command. Can be an absolute path are a relative one w.r.t. the path to the considered configuration file. Commands that should be executed relative to the current working dir can use the {EH\_WORKING\_DIR} pattern.
 
### enviroment
A list of _environment variables_ on the *host* that should be set before the commands are executed. See [exec-helper-config-environment](@ref exec-helper-config-environment)(5).

### env
A map of environment key/value pairs set *inside* the container. Default: an empty map.

### interactive
Boolean indicating whether to run interactively inside the container. Check the Docker documentation for more information. Default: same as the used Docker default.

### tty
Boolean indicating whether to use a pseudo-tty inside the container. Check the Docker documentation for more information. Default: same as the used Docker default.

### privileged
Boolean indicating whether to run the container in privileged mode. Check the Docker documentation for more information. Default: _no_.

### user
Set the given user *inside* the container. Check the Docker documentation for more information. Default: the container default.

## Settings for the _run_ mode
### volumes
List of volumes to be mounted into the container. Eeach value maps directly to a Docker volume configuration. Check the Docker documentation for all the options and formats that can be used. Default: an empty list.

### image
The Docker _image_ to use as the base image for creating a new container. This configuration option is _mandatory_ when the plugin is in _run_ mode.

## Settings for the _exec_ mode
### container
The Docker _container_ to execute the command in. Note that the container *must* already be running when this command is called. This configuration option is _mandatory_ when the plugin is in _exec_ mode.

# EXAMPLE
## Configuration
\include docker.example

## Usage
Save the example to an exec-helper configuration file and execute in the same directory:
\code{.sh}
eh example
\endcode

# SEE ALSO
See [exec-helper](@ref exec-helper)(1) for information about the usage of exec-helper.

See [exec-helper-config](@ref exec-helper-config)(5) for information about the configuration file format.

See [exec-helper-plugins](@ref exec-helper-plugins)(5) for information about the configuration file format.

# COPYRIGHT
Exec-helper is Copyright &copy; 2016-2020 under the Berkeley Software Distribution 3-clause (BSD 3-clause).

# AUTHOR
Written by B. Verhagen
