src = [
  'src/executionContent.cpp',
  'src/tmpFile.cpp',
  'src/configFileWriter.cpp',
  'src/generateRandom.cpp',
  'src/executionHandler.cpp',
  'src/execution.cpp',
  'src/configBuilder.cpp',
  'src/testCommand.cpp',
  'src/indirectStatement.cpp',
  'src/logger.cpp',
]

deps = [
  boost_serialization_dep,
  boost_filesystem_dep,
  yaml_cpp_dep,
  unittest
]

base_utils_lib = library('exec-helper-test-base-utils', src,
  include_directories : ['include', 'include/base-utils'],
  dependencies : deps,
)

test_base_utils = declare_dependency(
  include_directories: ['include'],
  link_with: base_utils_lib,
  dependencies: deps
)

subdir('application')
