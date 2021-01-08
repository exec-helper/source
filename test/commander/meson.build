src = [
  'src/commanderTest.cpp'
]

deps = [
  unittest,
  rapidcheck_dep,
  test_base_utils,
  test_utils,
  commander,
  config_generators,
  plugins_generators,
]

exe = executable('exec-helper-commander-unittest', src,
  dependencies: deps,
  install: true,
  install_dir: get_option('testdir')
)
test('commander', exe, suite: ['unittest'])
