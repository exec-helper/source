.. _exec-helper-config-patterns:

Patterns
********

Description
===========
Patterns are parts of the configuration that will be replaced by its value when evaluated by :program:`exec-helper`. The *patterns* keyword describes a list of patterns identified by their key. See the 'patterns' section for more information about how to define a pattern.

Patterns can be used to:

* add options to the :program:`exec-helper` command line. 
* allow iterating over multiple configurations
* control the configurations to iterate over

Patterns
========
A pattern can contain the following fields:

.. describe:: default-values

    A list of default values to use when no values have been defined.

.. describe:: short-option

    The short option on the command line associated with this pattern

.. describe:: long-option

    The long option on the command line associated with this pattern

Predefined patterns
===================
Exec-helper predefines some specific patterns for convenience:

* **EH_WORKING_DIR**: contains the working directory from where the :program:`exec-helper` executable is called.

Example configuration
=====================

.. literalinclude:: ../examples/exec-helper-config.example
    :language: yaml

See also
========
See :ref:`exec-helper-config` (5) for information about the configuration file.
