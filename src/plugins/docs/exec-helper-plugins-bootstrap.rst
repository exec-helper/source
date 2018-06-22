.. _exec-helper-plugins-bootstrap:

Bootstrap plugin
********************
Description
===========
The bootstrap is used for executing bootstrap files. This is often used in build chains.

Mandatory settings
==================
There are no mandatory settings for the bootstrap plugin.

Optional settings
=================
The configuration of the bootstrap plugin may contain the following settings:

.. program:: exec-helper-plugins-bootstrap

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: filename

    The name of the bootstrap script. Default: :code:`bootstrap.sh`.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/bootstrap.example
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
