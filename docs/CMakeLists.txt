option(BUILD_MAN_DOCUMENTATION "Create and install the MAN API documentation (requires Doxygen)" ON)
option(BUILD_HTML_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ON)
option(BUILD_XML_DOCUMENTATION "Create and install the XML based API documentation (requires Doxygen)" OFF)

if(BUILD_MAN_DOCUMENTATION OR BUILD_HTML_DOCUMENTATION OR BUILD_XML_DOCUMENTATION)
    find_package(Doxygen 1.8.15 REQUIRED)
    add_custom_target(docs ALL)

    if(BUILD_HTML_DOCUMENTATION)
        add_subdirectory(html)
    endif()

    if(BUILD_XML_DOCUMENTATION)
        add_subdirectory(xml)
    endif()

    if(BUILD_MAN_DOCUMENTATION)
        add_subdirectory(man)
    endif()
endif()
