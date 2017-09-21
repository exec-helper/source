set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Gzip REQUIRED)

set(MAN_EXTENSION ${MAN1_EXTENSION})
set(REFERENCED_PAGES ${MAN1_SOURCES} ${MAN5_SOURCES})      # Reference these pages in order to avoid Doxygen complaining about not finding them. Do not install them though.

# Create the input paths for the Doxyfile
PREPEND(MAN1_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN1_SOURCES_INTEGRATION} ${REFERENCED_PAGES})
string (REPLACE ";" " " MAN_SOURCES "${MAN1_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create the output paths
STRIP_PATHS(MAN1_STRIPPED ${MAN1_SOURCES_INTEGRATION})
PREPEND(MAN1_OUTPUT_TMP ${CMAKE_CURRENT_BINARY_DIR} ${MAN1_STRIPPED})
string (REPLACE ".md" "${MAN1_EXTENSION}" MAN1_OUTPUT_PAGES "${MAN1_OUTPUT_TMP}")

# Create the gzipped output paths
string(REPLACE "${MAN1_EXTENSION}" "${MAN1_EXTENSION}.gz" MAN1_OUTPUT_GZ "${MAN1_OUTPUT_PAGES}")

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_command(OUTPUT ${MAN1_OUTPUT_PAGES}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    MAIN_DEPENDENCY ${doxyfile_in}
    DEPENDS ${MAN1_SOURCES_FULL_PATH}
    VERBATIM)

FOREACH(FILE ${MAN1_OUTPUT_PAGES})
    add_custom_command(OUTPUT ${FILE}.gz
        COMMAND ${Gzip} --force ${FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${FILE}
        VERBATIM)
ENDFOREACH(FILE)

add_custom_target(docs-man-integration
    SOURCES ${MAN1_OUTPUT_GZ}
    COMMENT "Generating MAN1 integration documentation..."
    VERBATIM)

install(FILES ${MAN1_OUTPUT_GZ} DESTINATION share/man/man1 COMPONENT integration)
add_dependencies(docs-man1 docs-man-integration)
