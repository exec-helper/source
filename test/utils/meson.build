src = [
  'src/utils.cpp',
]

deps = [
  config,
  plugins,
  test_base_utils,
  test_stubs
]

utils_lib = library('exec-helper-test-utils', src,
  include_directories : ['include', 'include/utils'],
  dependencies : deps,
)

test_utils = declare_dependency(
  include_directories: ['include'],
  link_with: utils_lib,
  dependencies: deps
)
