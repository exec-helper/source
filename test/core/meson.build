core_generators = declare_dependency(
  include_directories: ['include'],
  dependencies: [rapidcheck_dep]
)

src = [
  'src/permutationIteratorTest.cpp',
  'src/posixShellTest.cpp',
  'src/taskTest.cpp',
  'src/immediateExecutorTest.cpp',
]

deps = [
  unittest,
  rapidcheck_dep,
  test_base_utils,
  test_utils,
  core,
  core_generators
]

exe = executable('exec-helper-core-unittest', src,
  dependencies: deps,
  install: true,
  install_dir: get_option('testdir')
)
test('core', exe, suite: ['unittest'])
