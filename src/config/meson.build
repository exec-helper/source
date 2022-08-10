src = [
  'src/settingsNode.cpp',
  'src/configFileSearcher.cpp',
  'src/logger.cpp',
  'src/fleetingOptions.cpp',
  'src/config.cpp',
  'src/pattern.cpp',
  'src/patternsHandler.cpp',
  'src/optionDescriptions.cpp',
  'src/envp.cpp',
  'src/cast.cpp',
  'src/pathManipulation.cpp',
  'src/yamlWrapper.cpp',
  'src/yaml.cpp',
  'src/logger.cpp',
]

deps = [
  log,
  yaml_cpp_dep,
  boost_program_options_dep
]

config_lib = library('exec-helper-config', src,
  include_directories : ['include', 'include/config'],
  dependencies : deps
)

config = declare_dependency(
  include_directories: ['include'],
  link_with: config_lib,
  dependencies: deps
)
