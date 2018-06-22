.. _exec-helper-plugins-pmd:

Pmd plugin
**********
Description
===========
The pmd plugin is used for executing the pmd static code analyzer tool suite.

Mandatory settings
==================
There are no mandatory settings for this plugin.

Optional settings
=================
The configuration of the pmd plugin may contain the following settings:

.. program:: exec-helper-plugins-pmd

.. include:: patterns.rst
 
.. include:: environment.rst

.. include:: command-line.rst

.. include:: working-dir.rst

.. describe:: exec

    The path to the pmd-run executable. The path can either be an absolute path or a relative path from the current working directory. Default: :code:`pmd`.

.. describe:: tool
    The pmd tool to use. The currently supported tools are:

      * cpd

    Default: :code:`cpd`

.. describe:: language

    Specify the language PMD is analyzing. Check the :code:`--language` option of the pmd documentation for more information. Default: no explicit language parameter is passed.

Cpd specific settings
---------------------

.. describe:: minimum-tokens

    The minimum token length to be considered a duplicate. Check the :code:`--minimum-tokens` option of the cpd documentation for more information. Default: no explicit minimum tokens parameter is passed.

.. describe:: files

    A list of files to check for duplicated code. Check the :code:`--files` option of the cpd documentation for more information. Default: no explicit files parameter is passed.

Example
=======
Configuration
-------------

.. literalinclude:: ../examples/pmd.example
    :language: yaml

Additional files
----------------
In order for the above example to work, the following file hierarchy needs to be created in the directory:

*Makefile*:

.. literalinclude:: ../examples/Makefile
    :language: none

*hello.cpp:*

.. literalinclude:: ../examples/src/hello.cpp
    :language: cpp

*world.cpp:*

.. literalinclude:: ../examples/src/world.cpp
    :language: cpp

Usage
-----
Save the example to an :program:`exec-helper` configuration file and execute in the same directory:

.. code-block:: bash

    eh example

.. include:: see-also.rst
