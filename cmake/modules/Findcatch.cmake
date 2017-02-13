# - Try to find LibXml2
# Once done this will define
#  LIBCATCH_FOUND - System has LibXml2
#  LIBCATCH_INCLUDE_DIRS - The LibXml2 include directories

set(MODULE_NAME catch)
set(MODULE_INCLUDES catch.hpp)

find_package(PkgConfig)
pkg_check_modules(${MODULE_NAME} QUIET ${MODULE_NAME})

find_path(${MODULE_NAME}_INCLUDE_DIR NAMES ${MODULE_INCLUDES} PATH_SUFFIXES catch)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${MODULE_NAME}  DEFAULT_MSG
                                  ${MODULE_NAME}_INCLUDE_DIR)

mark_as_advanced(${MODULE_NAME}_INCLUDE_DIR)

set(${MODULE_NAME}_INCLUDE_DIRS ${${MODULE_NAME}_INCLUDE_DIR} )
set(${MODULE_NAME}_LIBRARY_DIRS  ${${MODULE_NAME}_LIBRARY})
