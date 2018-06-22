.. _exec-helper-plugins-docker:

Docker plugin
*************
Description
===========
The Docker plugin is used for running or attaching to a Docker container.

Mandatory settings
==================
Mandatory settings change depending on which mode is selected. See *mode* for more information.

Optional settings
=================
The configuration of the make plugin may contain the following settings:

Settings for all modes
----------------------

.. program:: exec-helper-plugins-docker

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: mode

    Set the mode of the Docker call for the specific command. Default: :code:`exec`.

    Supported modes are:

    - **run**: Create a new container based on the given *image* and runs the given command. Note: use :code:`--rm` as an additional command line argument to automatically clean up the created container.
    - **exec**: Run the command in the given, actively running, *container*.
 
.. describe:: env

    A map of environment key/value pairs set *inside* the container. Default: an empty map.

.. describe:: interactive

    Boolean indicating whether to run interactively inside the container. Check the Docker documentation for more information. Default: same as the used Docker default.

.. describe::  tty

    Boolean indicating whether to use a pseudo-tty inside the container. Check the Docker documentation for more information. Default: same as the used Docker default.

.. describe:: privileged

    Boolean indicating whether to run the container in privileged mode. Check the Docker documentation for more information. Default: :code:`no`.

.. describe:: user

    Set the given user *inside* the container. Check the Docker documentation for more information. Default: the container default.

Settings for the *run* mode
---------------------------

.. program:: exec-helper-plugins-docker

.. describe:: volumes

    List of volumes to be mounted into the container. Eeach value maps directly to a Docker volume configuration. Check the Docker documentation for all the options and formats that can be used. Default: an empty list.

.. describe:: image

    The Docker *image* to use as the base image for creating a new container. This configuration option is *mandatory* when the plugin is in *run* mode.

Settings for the *exec* mode
----------------------------

.. program:: exec-helper-plugins-docker

.. describe:: container

    The Docker *container* to execute the command in. Note that the container *must* already be running when this command is called. This configuration option is *mandatory* when the plugin is in *exec* mode.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/docker.example
    :language: yaml

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
