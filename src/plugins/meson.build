src = [
  'src/luaPlugin.cpp',
  'src/pmd.cpp',
  'src/pluginUtils.cpp',
  'src/memory.cpp',
  'src/commandPlugin.cpp',
  'src/valgrind.cpp',
  'src/executePlugin.cpp',
  'src/plugin.cpp',
  'src/commandLineCommand.cpp',
  'src/lcov.cpp',
  'src/logger.cpp',
  'src/commandLine.cpp',
  'src/verbosity.cpp',
  'src/addEnvironment.cpp',
  'src/threadedness.cpp',
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
]
install_data(plugin_files, install_dir : get_option('datadir') / 'exec-helper' / 'plugins')