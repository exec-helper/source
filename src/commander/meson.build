src = [
  'src/commander.cpp',
  'src/logger.cpp',
]

deps = [
  log,
  plugins,
]

commander_lib = library('exec-helper-commander', src,
  include_directories : ['include', 'include/commander'],
  dependencies : deps,
)

commander = declare_dependency(
  include_directories: ['include'],
  link_with: commander_lib,
  dependencies: deps
)
