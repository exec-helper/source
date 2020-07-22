src = [
  'src/log.cpp',
  'src/logLevel.cpp',
  'src/logger.cpp',
  'src/consoleLogger.cpp'
]

deps = [
  boost_log_dep,
  gsl_dep
]

log_lib = library('exec-helper-log', src,
  include_directories: ['include', 'include/log'],
  dependencies: deps
)

log = declare_dependency(
  include_directories: ['include'],
  link_with: log_lib,
  dependencies: deps
)
