project('exec-helper', 'cpp',
  version: '0.7.0',
  default_options: [
    'warning_level=3',
    'cpp_std=c++20',
    'cpp_rtti=true',
    'default_library=static'
  ]
)

if get_option('terminate-on-assert')
  add_project_arguments('-DTERMINATE_ON_ASSERT_FAILURE', language: 'cpp')
endif

if meson.get_compiler('cpp').get_id() == 'gcc'
  add_project_arguments('-Wno-maybe-uninitialized', language: 'cpp')
endif

cmake = import('cmake')

yaml_cpp_dep = dependency('yaml-cpp', required : get_option('use-system-yaml-cpp'))
if not yaml_cpp_dep.found()
  yaml_cpp_proj = cmake.subproject('yaml-cpp', cmake_options: ['-DYAML_CPP_BUILD_CONTRIB=OFF', '-DYAML_CPP_BUILD_TOOLS=OFF', '-DBUILD_SHARED_LIBS=OFF', '-DMSVC_SHARED_RT=ON', '-DMSVC_STHREADED_RT=OFF', '-DAPPLE_UNIVERSAL_BIN=OFF', '-DYAML_CPP_BUILD_TESTS=OFF', '-DCMAKE_POSITION_INDEPENDENT_CODE=ON'])
  yaml_cpp_dep = yaml_cpp_proj.dependency('yaml_cpp')
endif

lua_dep = dependency('lua5.3', required: false)
if not lua_dep.found()
  lua_dep = dependency('lua-5.3', required: get_option('use-system-lua'))
endif
if not lua_dep.found()
  lua_proj = subproject('lua', default_options: ['default_library=static', 'werror=false'])
  lua_dep = lua_proj.get_variable('lua_dep')
endif

luawrapper = declare_dependency(
  include_directories: ['3rdparty/luawrapper'],
  dependencies: lua_dep,
)

boost_dep = dependency('boost', version: '>=1.64.0', static: get_option('use-static-boost'))
boost_filesystem_dep = dependency('boost', modules: ['filesystem'], static: get_option('use-static-boost'))
boost_program_options_dep = dependency('boost', modules: ['program_options'], static: get_option('use-static-boost'))
boost_log_dep = dependency('boost', modules: ['log', 'log_setup', 'thread', 'filesystem'], static: get_option('use-static-boost'))

# Fix for boost for meson version 0.53 and lower.
if get_option('use-static-boost')
  add_project_arguments(['-DBOOST_ALL_STATIC_LINK=1', '-DBOOST_LOG_STATIC_LINK=1', '-DBOOST_PROGRAM_OPTIONS_STATIC_LINK=1', '-DBOOST_FILESYSTEM_STATIC_LINK=1'], language: 'cpp')
else
  add_project_arguments(['-DBOOST_ALL_DYN_LINK=1', '-DBOOST_LOG_DYN_LINK=1', '-DBOOST_PROGRAM_OPTIONS_DYN_LINK=1', '-DBOOST_FILESYSTEM_DYN_LINK=1'], language: 'cpp')
endif

if host_machine.system() == 'windows'
  add_project_arguments(['-D_WIN32_WINNT=0x0601', '-D_CRT_SECURE_NO_WARNINGS'], language: 'cpp')
endif

install_data('LICENSE', install_dir : get_option('license-prefix'))

subdir('gitchangelog')
subdir('docs')
subdir('src')

if get_option('test')
  boost_serialization_dep = dependency('boost', modules : ['serialization'])

  catch2_dep = dependency('catch2', required: get_option('use-system-catch2'))
  if not catch2_dep.found()
    catch2_proj = subproject('catch2')
    catch2_dep = catch2_proj.get_variable('catch2_dep')
  endif

  rapidcheck_dep = dependency('rapidcheck', required: get_option('use-system-rapidcheck'))
  if not rapidcheck_dep.found()
    rapidcheck_proj = cmake.subproject('rapidcheck', cmake_options: ['-DRC_ENABLE_CATCH=OFF', '-DRC_ENABLE_TESTS=OFF', '-DBUILD_TESTING=OFF', '-DCATCH_BUILD_TESTING=OFF', '-DCATCH_ENABLE_WERROR=OFF', '-DCATCH_INSTALL_DOCS=OFF', '-DCATCH_INSTALL_HELPERS=OFF', '-DCMAKE_POSITION_INDEPENDENT_CODE=ON'])
    rapidcheck_dep = rapidcheck_proj.dependency('rapidcheck')
  endif

  subdir('test')
endif

if get_option('completion-files')
  subdir('completion')
endif

summary({
  'Prefix': get_option('prefix'),
  'Plugins prefix': get_option('plugins-prefix'),
  'License prefix': get_option('license-prefix')
}, section: 'Directories')

summary({
  'Version': get_option('version'),
  'Copyright notice': get_option('copyright'),
  'Build tests': get_option('test'),
  'Build usage documentation': get_option('usage-documentation'),
  'Build api documentation': get_option('api-documentation'),
  'Completion files': get_option('completion-files'),
}, section: 'Configuration', bool_yn: true)

if get_option('usage-documentation')
  summary({
  'Build usage man documentation': get_option('usage-documentation-man'),
  'Build usage html documentation': get_option('usage-documentation-html'),
}, section: 'Usage documentation', bool_yn: true)

endif
