set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Gzip REQUIRED)

set(MAN_EXTENSION ${MAN5_EXTENSION})
set(REFERENCED_PAGES ${MAN1_SOURCES})      # Reference these pages in order to avoid Doxygen complaining about not finding them. Do not install them though.

# Create the input paths for the Doxyfile
PREPEND(MAN5_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN5_SOURCES} ${REFERENCED_PAGES})
string (REPLACE ";" " " MAN_SOURCES "${MAN5_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create the output paths
STRIP_PATHS(MAN5_STRIPPED ${MAN5_SOURCES})
PREPEND(MAN5_OUTPUT_TMP ${CMAKE_CURRENT_BINARY_DIR} ${MAN5_STRIPPED})
string (REPLACE ".md" "${MAN5_EXTENSION}" MAN5_OUTPUT_PAGES "${MAN5_OUTPUT_TMP}")

# Create the gzipped output paths
string(REPLACE "${MAN5_EXTENSION}" "${MAN5_EXTENSION}.gz" MAN5_OUTPUT_GZ "${MAN5_OUTPUT_PAGES}")

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_command(OUTPUT ${MAN5_OUTPUT_PAGES}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    MAIN_DEPENDENCY ${doxyfile_in}
    DEPENDS ${MAN5_SOURCES_FULL_PATH}
    VERBATIM)

FOREACH(FILE ${MAN5_OUTPUT_PAGES})
    add_custom_command(OUTPUT ${FILE}.gz
        COMMAND ${Gzip} --force ${FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${FILE}
        VERBATIM)
ENDFOREACH(FILE)

add_custom_target(docs-man5
    SOURCES ${MAN5_OUTPUT_GZ}
    COMMENT "Generating MAN5 API documentation..."
    VERBATIM)

install(FILES ${MAN5_OUTPUT_GZ} DESTINATION share/man/man5 COMPONENT docs-man)

add_dependencies(docs-man docs-man5)
