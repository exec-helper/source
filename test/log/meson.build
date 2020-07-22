log_generators = declare_dependency(
  include_directories: ['include'],
  dependencies: [rapidcheck_dep]
)

src = [
  'src/logTest.cpp'
]

deps = [
  unittest,
  rapidcheck_dep,
  log,
  log_generators
]

exe = executable('exec-helper-log-unittest', src,
  dependencies: deps,
  install: true,
  install_dir: get_option('testdir')
)
test('log', exe, suite: ['unittest'])
