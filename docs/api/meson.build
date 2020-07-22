doxygen = find_program('doxygen')

conf_data = configuration_data()
conf_data.set('DOXYGEN_QUIET', 'YES')
conf_data.set('GENERATE_HTML', 'YES')
conf_data.set('GENERATE_XML', 'YES')
conf_data.set('PROJECT_SOURCE_DIR', meson.source_root())
conf_data.set('PROJECT_BINARY_DIR', meson.build_root())

if get_option('werror')
  conf_data.set('WARN_AS_ERROR', 'YES')
else
  conf_data.set('WARN_AS_ERROR', 'NO')
endif

doxygen_config = configure_file(
  input: 'Doxyfile.in',
  output: 'Doxyfile',
  configuration: conf_data
)

api = custom_target('api documentation',
  command: [doxygen, '@INPUT@'],
  input: doxygen_config,
  output: ['xml', 'html'],
  build_by_default: true,
  install: true,
  install_dir : get_option('datadir') / 'doc' / 'exec-helper' / 'api'
)

docs_aliases += api
