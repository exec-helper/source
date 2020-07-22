config_generators = declare_dependency(
  include_directories: ['include'],
  dependencies: [rapidcheck_dep]
)

src = [
  'src/settingsNodeTest.cpp',
  'src/configFileSearcherTest.cpp',
  'src/patternTest.cpp',
  'src/patternHandlerTest.cpp',
  'src/configTest.cpp',
  'src/fleetingOptionsTest.cpp',
  'src/commandLineOptionsTest.cpp',
  'src/argvTest.cpp',
  'src/envpTest.cpp',
  'src/pathManipulationTest.cpp',
  'src/yamlTest.cpp',
]

deps = [
  unittest,
  rapidcheck_dep,
  test_base_utils,
  test_utils,
  config,
  log_generators,
  config_generators
]

exe = executable('exec-helper-config-unittest', src,
  dependencies: deps,
  install: true,
  install_dir: get_option('testdir')
)
test('config', exe, suite: ['unittest'])
