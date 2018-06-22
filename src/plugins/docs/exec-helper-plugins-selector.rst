.. _exec-helper-plugins-selector:

Selector plugin
***************
Description
The selector plugin is used for selecting a configuration path based on the value(s) of a target, typically one from a pattern value.

Mandatory settings
==================
The configuration of the command-line-command must contain the following settings:

.. program:: exec-helper-plugins-selector

.. include:: patterns.rst

.. describe:: targets

    The targets to select on. Note that if patterns are used in this list, they must be listed using the *patterns* configuration, as is the case for every plugin.

    The runtime value(s) associated with the pattern key must resolve either to an existing (configured) plugin or a configured command.

Optional settings
=================
There are no optional settings for the selector plugin.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/selector.example
    :language: yaml

Usage
-----
Save the example to an :program:`exec-helper` configuration file and execute in the same directory:

.. code-block:: bash

    eh example

.. include:: see-also.rst
