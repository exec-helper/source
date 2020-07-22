src = [
  'src/task.cpp',
  'src/immediateExecutor.cpp',
  'src/reportingExecutor.cpp',
  'src/patterns.cpp',
  'src/posixShell.cpp',
  'src/logger.cpp',
]

deps = [
  boost_filesystem_dep,
  log,
  config
]

core_lib = library('exec-helper-core', src,
  include_directories : ['include', 'include/core'],
  dependencies : deps,
)

core = declare_dependency(
  include_directories: ['include'],
  link_with: core_lib,
  dependencies: deps
)
