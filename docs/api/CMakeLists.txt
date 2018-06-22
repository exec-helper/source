include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(BUILD_API_HTML_DOCUMENTATION "Create and install the HTML API documentation (requires Doxygen)" ON "BUILD_API_DOCUMENTATION" OFF)
CMAKE_DEPENDENT_OPTION(BUILD_API_XML_DOCUMENTATION "Create and install the MAN API documentation (requires Doxygen)" ON "BUILD_API_DOCUMENTATION" OFF)

find_package(Doxygen 1.8.15 REQUIRED)

if(CMAKE_VERBOSE_MAKEFILE)
    set(DOXYGEN_QUIET NO)
else()
    set(DOXYGEN_QUIET YES)
endif()

if(ENABLE_WERROR)
    set(WARN_AS_ERROR YES)
else()
    set(WARN_AS_ERROR NO)
endif()

if(BUILD_API_HTML_DOCUMENTATION)
    add_custom_target(docs-api-html ALL)
    set(GENERATE_HTML "YES")
else()
    set(GENERATE_HTML "NO")
endif()

if(BUILD_API_XML_DOCUMENTATION)
    add_custom_target(docs-api-xml ALL)
    set(GENERATE_XML "YES")
else()
    set(GENERATE_XML "NO")
endif()

set(doxyfile_in Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(docs-api
    SOURCES ${doxyfile_in}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html DESTINATION ${CMAKE_INSTALL_DOCDIR}/api COMPONENT docs-api-html)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/xml DESTINATION ${CMAKE_INSTALL_DOCDIR}/api COMPONENT docs-api-xml)

add_dependencies(docs docs-api)

if(BUILD_API_HTML_DOCUMENTATION)
    add_dependencies(docs-api-html docs-api)
endif()

if(BUILD_API_XML_DOCUMENTATION)
    add_dependencies(docs-api-xml docs-api)
endif()
