# - Try to find LibXml2
# Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  LIBXML2_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

set(MODULE_NAME yaml-cpp)
set(MODULE_INCLUDES yaml-cpp/yaml.h)
set(MODULE_LIB yaml-cpp)

find_package(PkgConfig)
pkg_check_modules(${MODULE_NAME} QUIET ${MODULE_NAME})

find_path(${MODULE_NAME}_INCLUDE_DIR NAMES ${MODULE_INCLUDES})

find_library(${MODULE_NAME}_LIBRARY NAMES ${MODULE_LIB})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${MODULE_NAME}  DEFAULT_MSG
                                  ${MODULE_NAME}_INCLUDE_DIR ${MODULE_NAME}_LIBRARY)

mark_as_advanced(${MODULE_NAME}_INCLUDE_DIR ${MODULE_NAME}_LIBRARY)

set(${MODULE_NAME}_INCLUDE_DIRS ${${MODULE_NAME}_INCLUDE_DIR} )
set(${MODULE_NAME}_LIBRARIES ${MODULE_LIB} )
set(${MODULE_NAME}_LIBRARY_DIRS  ${${MODULE_NAME}_LIBRARY})
