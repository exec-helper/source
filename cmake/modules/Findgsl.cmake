set(MODULE_NAME gsl)
set(MODULE_INCLUDES gsl)

find_path(gsl_INCLUDE_DIRS NAMES gsl PATH_SUFFIXES gsl)

# This module is header only
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gsl DEFAULT_MSG
                                  gsl_INCLUDE_DIRS)

if(gsl_FOUND)
    add_library(gsl STATIC IMPORTED)

    set_target_properties(gsl PROPERTIES    
        INTERFACE_INCLUDE_DIRECTORIES "${gsl_INCLUDE_DIRS}"
    )
endif()

#mark_as_advanced(${MODULE_NAME}_INCLUDE_DIR)

#set(gsl_INCLUDE_DIRS /usr/lib/gsl)
#set(gsl_LIBRARIES gsl)

#if(${MODULE_NAME}_FOUND)
    #add_library(gsl::gsl UNKNOWN IMPORTED)
    #set_target_properties(gsl::gsl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${${MODULE_NAME}_INCLUDE_DIRS}")
    #set_property(TARGET gsl::gsl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)    
    #set_target_properties(gsl::gsl PROPERTIES IMPORTED_LOCATION_RELEASE "")
#endif()
