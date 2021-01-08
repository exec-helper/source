# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))
import sphinx_rtd_theme

# -- Project information -----------------------------------------------------

project = 'Exec-helper'
copyright = 'Exec-helper is Copyright (C) 2016-2020 under the Berkeley Software Distribution 3-clause (BSD 3-clause).'
author = 'Bart Verhagen'

section = '5'

# The full version, including alpha/beta/rc tags
release = '0.5.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    # 'recommonmark',
    'sphinx_rtd_theme'
]

master_doc = 'index'
templates_path = ['docs/usage/_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'docs/sphinx']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['docs/usage/_static']

highlight_language = 'yaml'

# -- Options for man output --
man_pages = [
    ('src/applications/exec-helper', 'exec-helper', 'Or How To Get Your Coffee In Peace: A meta-wrapper for executables', '', 1),
    ('src/applications/exec-helper', 'eh', 'Or How To Get Your Coffee In Peace: A meta-wrapper for executables', '', 1),
    ('src/config/docs/exec-helper-config', 'exec-helper-config', 'Exec-helper configuration documentation', '', 5),
    ('src/config/docs/exec-helper-config-patterns', 'exec-helper-config-patterns', 'Using patterns', '', 5),
    ('src/config/docs/exec-helper-config-environment', 'exec-helper-config-environment', 'Configuring environments', '', 5),
    ('src/plugins/docs/exec-helper-plugins', 'exec-helper-plugins', 'A list of included plugins', '', 5),
    ('src/plugins/docs/exec-helper-custom-plugins', 'exec-helper-custom-plugins', 'Writing custom plugins', '', 5),
    ('src/plugins/docs/exec-helper-plugins-bootstrap', 'exec-helper-plugins-bootstrap', 'An overview of the bootstrap plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-clang-static-analyzer', 'exec-helper-plugins-clang-static-analyzer', 'An overview of the clang-static-analyzer plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-clang-tidy', 'exec-helper-plugins-clang-tidy', 'An overview of the clang-tidy plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-cmake', 'exec-helper-plugins-cmake', 'An overview of the cmake plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-command-line-command', 'exec-helper-plugins-command-line-command', 'An overview of the command-line-command plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-cppcheck', 'exec-helper-plugins-cppcheck', 'An overview of the cppcheck plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-docker', 'exec-helper-plugins-docker', 'An overview of the docker plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-execute-plugin', 'exec-helper-plugins-execute-plugin', 'An overview of the execute-plugin plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-lcov', 'exec-helper-plugins-lcov', 'An overview of the lcov plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-make', 'exec-helper-plugins-make', 'An overview of the make plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-ninja', 'exec-helper-plugins-ninja', 'An overview of the ninja plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-pmd', 'exec-helper-plugins-pmd', 'An overview of the pmd plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-scons', 'exec-helper-plugins-scons', 'An overview of the scons plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-selector', 'exec-helper-plugins-selector', 'An overview of the selector plugin configuration settings', '', 5),
    ('src/plugins/docs/exec-helper-plugins-valgrind', 'exec-helper-plugins-valgrind', 'An overview of the valgrind plugin configuration settings', '', 5),
]
