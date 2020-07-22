src = [
  'exec-helper.cpp',
  'logger.cpp',
]

deps = [
  boost_program_options_dep,
  log,
  commander,
]

version_data = configuration_data()
version_data.set('BINARY_NAME', 'exec-helper')
version_data.set('VERSION', get_option('version'))
version_data.set('COPYRIGHT', get_option('copyright'))
version_data.set('ACTUAL_PLUGINS_INSTALL_PREFIX', get_option('plugins-prefix'))

configure_file(input: 'version.h.in',
               output: 'version.h',
               configuration: version_data)

exec_helper = executable('exec-helper', src,
  dependencies: deps,
  install : true,
)

if host_machine.system() == 'windows'
    # Make a full copy on Windows
    executable('eh', src,
      dependencies: deps,
      install : true,
    )
else
    # Create a symlink
  custom_target('eh', 
    input: exec_helper,
    output: 'eh',
    command: ['ln', '--symbolic', '--force', '@PLAINNAME@', '@OUTPUT@'],
    install: true,
    install_dir: get_option('bindir')
  )
endif
