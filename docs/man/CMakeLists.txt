find_package(Doxygen)

option(BUILD_MAN_DOCUMENTATION "Create and install the MAN API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(FILEPATH ${ARGN})
       LIST(APPEND listVar "${prefix}/${FILEPATH}")
   ENDFOREACH(FILEPATH)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

FUNCTION(STRIP_PATHS var)
   SET(listVar "")
   FOREACH(FILEPATH ${ARGN})
       get_filename_component(STRIPPED ${FILEPATH} NAME)
      LIST(APPEND listVar ${STRIPPED})
  ENDFOREACH(FILEPATH)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(STRIP_PATHS)

if(BUILD_MAN_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required to build the documentation.")
    endif()

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
    find_package(Gzip REQUIRED)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required to build the documentation.")
    endif()

    add_custom_target(docs-man COMMENT "Building man-page API documentation" )
    add_dependencies(docs docs-man)

    add_subdirectory(man1)
    add_subdirectory(man5)
endif()
