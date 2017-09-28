set(MODULE_NAME Gsl)
set(MODULE_INCLUDES gsl)

find_package(PkgConfig)
pkg_check_modules(${MODULE_NAME} QUIET ${MODULE_NAME})

if(NOT ${MODULE_NAME}_FOUND)
    find_path(${MODULE_NAME}_INCLUDE_DIR NAMES ${MODULE_INCLUDES} PATH_SUFFIXES gsl)

    # This module is header only
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(${MODULE_NAME} DEFAULT_MSG
                                      ${MODULE_NAME}_INCLUDE_DIR)

    mark_as_advanced(${MODULE_NAME}_INCLUDE_DIR)
endif()
