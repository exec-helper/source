find_program(GZIP NAMES gzip PATHS /bin /usr/bin /usr/local/bin)

mark_as_advanced(GZIP)

set(${MODULE_NAME}_INCLUDE_DIRS ${${MODULE_NAME}_INCLUDE_DIR} )
set(${MODULE_NAME}_LIBRARY_DIRS  ${${MODULE_NAME}_LIBRARY})
