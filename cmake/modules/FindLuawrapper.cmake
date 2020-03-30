set(PKG_NAME Luawrapper)

find_package(PkgConfig QUIET)
if(${PKG_CONFIG_FOUND})
    pkg_check_modules(${PKG_NAME} luawrapper)
endif()

# Attempt to find it if not configured in pkgconfig
if(NOT ${PKG_NAME}_FOUND)
    MESSAGE(STATUS "Looking manually")
    find_path(${PKG_NAME}_INCLUDE_DIRS NAMES LuaContext.hpp PATH_SUFFIXES luawrapper)

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(${PKG_NAME}
                                      FOUND_VAR ${PKG_NAME}_FOUND
                                      REQUIRED_VARS ${PKG_NAME}_INCLUDE_DIRS
                                      )

    mark_as_advanced(${PKG_NAME}_INCLUDE_DIRS)
endif()
