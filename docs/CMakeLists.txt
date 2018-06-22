option(BUILD_USAGE_DOCUMENTATION "Create and install the usage documentation (requires Sphinx)" ON)
option(BUILD_API_DOCUMENTATION "Create and install the API documentation (requires Doxygen)" OFF)

add_custom_target(docs ALL)

if(BUILD_USAGE_DOCUMENTATION)
    message(STATUS "Building usage documentation")
    add_subdirectory(usage)
endif()

if(BUILD_API_DOCUMENTATION)
    message(STATUS "Building API documentation")
    add_subdirectory(api)
endif()
