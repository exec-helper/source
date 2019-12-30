set(PKG_NAME integral)

find_package(PkgConfig QUIET)
if(${PKG_CONFIG_FOUND})
    pkg_check_modules(${PKG_NAME} integral)
endif()

# Attempt to find it if not configured in pkgconfig
if(NOT ${PKG_NAME}_FOUND)
    MESSAGE(STATUS "Looking manually")
    set(${PKG_NAME}_LIBRARIES integral)
    find_path(${PKG_NAME}_INCLUDE_DIRS NAMES integral.h PATH_SUFFIXES integral)
    find_library(${PKG_NAME}_LIBRARY_DIRS NAMES ${${PKG_NAME}_LIBRARIES})

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(${PKG_NAME}
                                      FOUND_VAR ${PKG_NAME}_FOUND
                                      REQUIRED_VARS ${PKG_NAME}_INCLUDE_DIRS ${PKG_NAME}_LIBRARY_DIRS
                                      )

    mark_as_advanced(${PKG_NAME}_INCLUDE_DIRS)
    mark_as_advanced(${PKG_NAME}_LIBRARIES)
    mark_as_advanced(${PKG_NAME}_LIBRARY_DIRS)
endif()
