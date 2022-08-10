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
  'src/scripts/make.lua',
  'src/scripts/cmake.lua',
  'src/scripts/docker.lua',
  'src/scripts/ninja.lua',
  'src/scripts/bootstrap.lua',
  'src/scripts/scons.lua',
  'src/scripts/clang-tidy.lua',
  'src/scripts/cppcheck.lua',
  'src/scripts/selector.lua',
  'src/scripts/clang-static-analyzer.lua',
  'src/scripts/pmd.lua',
  'src/scripts/valgrind.lua',
  'src/scripts/lcov.lua',
  'src/scripts/meson.lua',
  'src/scripts/sh.lua',
  'src/scripts/bash.lua',
  'src/scripts/zsh.lua',
  'src/scripts/fish.lua',
]
install_data(plugin_files, install_dir : get_option('datadir') / 'exec-helper' / 'plugins')
