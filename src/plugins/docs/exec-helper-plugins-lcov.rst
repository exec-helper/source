.. _exec-helper-plugins-lcov:

Lcov plugin
***********
Description
===========
The lcov plugin is used for executing code coverage analysis using lcov.

Mandatory settings
==================
The configuration of the lcov plugin must contain the following settings:

.. program:: exec-helper-plugins-lcov

.. describe:: run-command

    The :program:`exec-helper` command or plugin to use for running the binaries for which the coverage needs to be analyzed.

Optional settings
=================
The configuration of the lcov plugin may contain the following settings:

.. program:: exec-helper-plugins-lcov

.. include:: patterns.rst

.. include:: command-line.rst

.. describe:: info-file

    The lcov .info file to use for the analysis. Default: :code:`lcov-plugin.info`.

.. describe:: base-directory

    The base directory to use for the lcov analysis. Check the lcov documentation on the :code:`--base-directory` option for more information. Default: :code:`.` (the current working directory).

.. describe:: directory

    Use the coverage data files in the given directory. Check the lcov documentation on the :code:`--directory` option for more information. Default: :code:`.` (the current working directory).

.. describe:: zero-counters

    Set this option to *yes* to reset the coverage counters before starting the analysis. All other values are threated as *no*. Default: :code:`no`.

.. describe:: gen-html

    Set this option to *yes* to enable HTML report generation of the coverage data. Default: :code:`no`.

.. describe:: gen-html-output

    Set the output directory of the generated HTML report. Does nothing if **gen-html** is not enabled. Default: :code:`.` (the current working directory).

.. describe:: gen-html-title

    Set the title of the generated HTML report. Does nothing if **gen-html** is not enabled. Default: :code:`Hello`.

.. describe:: gen-html-command-line

    Set additional command line options for the gen html stage. Default: no additional command line options.

.. describe:: excludes

    A list of directories and files to excluse from the coverage report. The paths are relative to the current working directory. Default: an empty list.
 
Example
=======
Configuration
-------------

.. literalinclude:: ../examples/lcov.example
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

See also
========
See :ref:`exec-helper` (1) for information about the usage of :program:`exec-helper`.

See :ref:`exec-helper-config` (5) for information about the configuration file format.

See :ref:`exec-helper-plugins` (5) for information about the configuration file format.
