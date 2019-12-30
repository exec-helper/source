set(MODULE_NAME luajit)

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(${MODULE_NAME} luajit IMPORTED_TARGET)
endif()

# Attempt to find it if not configured in pkgconfig
if(NOT ${MODULE_NAME}_FOUND)
    MESSAGE(STATUS "Looking manually")
    set(${MODULE_NAME}_LIBRARIES luajit-5.1)
    find_path(${MODULE_NAME}_INCLUDE_DIRS NAMES lua.hpp PATH_SUFFIXES luajit-2.0 luajit-2.1)
    find_library(${MODULE_NAME}_LIBRARY_DIRS NAMES ${${MODULE_NAME}_LIBRARIES})

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(${MODULE_NAME}
                                      FOUND_VAR ${MODULE_NAME}_FOUND
                                      REQUIRED_VARS ${MODULE_NAME}_INCLUDE_DIRS ${MODULE_NAME}_LIBRARY_DIRS
                                      )

    mark_as_advanced(${MODULE_NAME}_INCLUDE_DIRS)
    mark_as_advanced(${MODULE_NAME}_LIBRARIES)
    mark_as_advanced(${MODULE_NAME}_LIBRARY_DIRS)

    if(${MODULE_NAME}_FOUND)
        add_library(${MODULE_NAME} SHARED IMPORTED GLOBAL)
        target_include_directories(${MODULE_NAME} SYSTEM INTERFACE ${${MODULE_NAME}_INCLUDE_DIRS})
        target_link_libraries(${MODULE_NAME} INTERFACE ${${MODULE_NAME}_LIBRARIES})
    endif()
endif()
