# add a target to generate API documentation with Doxygen
option(BUILD_HTML_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_HTML_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(HTML_INDEX_FILE index.html)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_command(OUTPUT ${HTML_INDEX_FILE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${doxyfile_in}
        VERBATIM)

    add_custom_target(docs-html
        SOURCES ${HTML_INDEX_FILE}
        COMMENT "Generating API HTML documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DESTINATION usr/share/doc/exec-helper COMPONENT docs-html)

    add_dependencies(docs docs-html)
endif()
