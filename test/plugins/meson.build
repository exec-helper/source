plugins_generators = declare_dependency(
  include_directories: ['include'],
  dependencies: [rapidcheck_dep]
)

src = [
  'src/genericPluginTest.cpp',
  'src/clangStaticAnalyzerTest.cpp',
  'src/clangTidyTest.cpp',
  'src/pmdTest.cpp',
  'src/commandLineCommandTest.cpp',
  'src/cppcheckTest.cpp',
  'src/pluginUtilsTest.cpp',
  'src/bootstrapTest.cpp',
  'src/executePluginTest.cpp',
  'src/valgrindTest.cpp',
  'src/selectorTest.cpp',
  'src/sconsTest.cpp',
  'src/makeTest.cpp',
  'src/lcovTest.cpp',
  'src/ninjaTest.cpp',
  'src/cmakeTest.cpp',
  'src/dockerTest.cpp',
  'src/mesonTest.cpp',
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
