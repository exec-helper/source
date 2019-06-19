set(GENERATE_HTML "YES")

set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(HTML_INDEX_FILE index.html)

if(CMAKE_VERBOSE_MAKEFILE)
    set(DOXYGEN_QUIET NO)
else()
    set(DOXYGEN_QUIET YES)
endif()

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

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DESTINATION share/doc/exec-helper COMPONENT docs-html)

add_dependencies(docs docs-html)
