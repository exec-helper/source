if(BUILD_XML_DOCUMENTATION)
    set(GENERATE_XML "YES")
endif()

set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(XML_INDEX_FILE index.xml)

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_command(OUTPUT ${XML_INDEX_FILE}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${doxyfile_in}
    VERBATIM)

add_custom_target(docs-xml
    SOURCES ${XML_INDEX_FILE}
    COMMENT "Generating API XML documentation with Doxygen"
    VERBATIM)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} DESTINATION share/doc/exec-helper COMPONENT docs-xml)

add_dependencies(docs docs-xml)
