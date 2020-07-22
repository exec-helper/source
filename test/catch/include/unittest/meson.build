config_data = configuration_data()
config_data.set('BINARY_NAME', 'exec-helper')
config_data.set('VERSION', 'DEVELOPER-VERSION')
config_data.set('COPYRIGHT', 'COPYRIGHT (c)')
config_data.set('ACTUAL_PLUGINS_INSTALL_PREFIX', get_option('plugins-prefix'))

configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: config_data)
