plugins_generators = declare_dependency(
  include_directories: ['include'],
  dependencies: [rapidcheck_dep]
)

src = [
  'src/genericPluginTest.cpp',
  'src/commandLineCommandTest.cpp',
  'src/pluginUtilsTest.cpp',
  'src/executePluginTest.cpp',
  'src/selectorTest.cpp',
  'src/shellTest.cpp',
]

deps = [
  unittest,
  rapidcheck_dep,
  test_base_utils,
  test_utils,
  plugins,
  log_generators,
  config_generators,
  core_generators,
  plugins_generators
]

exe = executable('exec-helper-plugins-unittest', src,
  include_directories: ['include/plugins'],
  dependencies: deps,
  install: true,
  install_dir: get_option('testdir')
)
test('plugins', exe, suite: ['unittest'])
