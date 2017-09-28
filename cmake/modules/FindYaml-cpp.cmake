set(MODULE_NAME Yaml-cpp)

find_package(PkgConfig QUIET)
if(${PKG_CONFIG_FOUND})
    pkg_check_modules(${MODULE_NAME} yaml-cpp)
endif()

# Attempt to find it if not configured in pkgconfig
if(NOT ${${MODULE_NAME}_FOUND})
    MESSAGE(STATUS "Looking manually")
    set(${MODULE_NAME}_LIBRARIES yaml-cpp)
    find_path(${MODULE_NAME}_INCLUDE_DIRS NAMES yaml.h PATH_SUFFIXES yaml-cpp)
    find_library(${MODULE_NAME}_LIBRARY_DIRS NAMES ${${MODULE_NAME}_LIBRARIES})

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(${MODULE_NAME}
                                      FOUND_VAR ${MODULE_NAME}_FOUND
                                      REQUIRED_VARS ${MODULE_NAME}_INCLUDE_DIRS ${MODULE_NAME}_LIBRARY_DIRS
                                      )

    mark_as_advanced(${MODULE_NAME}_INCLUDE_DIRS)
    mark_as_advanced(${MODULE_NAME}_LIBRARIES)
    mark_as_advanced(${MODULE_NAME}_LIBRARY_DIRS)
endif()
