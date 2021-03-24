src = [
  'src/luaPlugin.cpp',
  'src/pluginUtils.cpp',
  'src/executePlugin.cpp',
  'src/commandLineCommand.cpp',
  'src/logger.cpp',
  'src/commandLine.cpp',
  'src/addEnvironment.cpp',
]

deps = [
  log,
  config,
  core,
  luawrapper,
  gsl_dep
]

plugins_lib = library('exec-helper-plugins', src,
  include_directories : ['include', 'include/plugins'],
  dependencies : deps,
)

plugins = declare_dependency(
  include_directories: ['include'],
  link_with: plugins_lib,
  dependencies: deps
)

plugin_files = [
  'src/scripts/selector.lua',
  'src/scripts/sh.lua',
  'src/scripts/bash.lua',
]
install_data(plugin_files, install_dir : get_option('datadir') / 'exec-helper' / 'plugins')
