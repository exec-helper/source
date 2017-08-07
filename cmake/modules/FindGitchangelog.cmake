set(MODULE_NAME Gitchangelog)
set(EXE_NAME gitchangelog)

if(NOT ${MODULE_NAME}_FOUND)
    find_program(${MODULE_NAME} NAMES ${EXE_NAME} PATHS /bin /usr/bin /usr/local/bin)

    mark_as_advanced(${MODULE_NAME})

    if(${MODULE_NAME} EQUAL ${MODULE_NAME}-NOTFOUND)
        MESSAGE(STATUS "Could NOT find " ${MODULE_NAME})
        set(${MODULE_NAME}_FOUND FALSE)
        unset(${MODULE_NAME})
    else()
        MESSAGE(STATUS "Found " ${MODULE_NAME} ": " ${${MODULE_NAME}})
        set(${MODULE_NAME}_FOUND TRUE)
    endif()
endif()
