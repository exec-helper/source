set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(MAN1_SOURCES    src/applications/exec-helper.md
    )
set(MAN1_EXTENSION .1)
set(MAN_EXTENSION ${MAN1_EXTENSION})

# Create the input paths for the Doxyfile
PREPEND(MAN1_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN1_SOURCES})
string (REPLACE ";" " " MAN_SOURCES "${MAN1_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create the output paths
STRIP_PATHS(MAN1_STRIPPED ${MAN1_SOURCES})
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
        COMMAND ${GZIP} --force ${FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${FILE}
        VERBATIM)
ENDFOREACH(FILE)

add_custom_target(docs-man1
    SOURCES ${MAN1_OUTPUT_GZ}
    COMMENT "Generating MAN1 API documentation..."
    VERBATIM)

install(FILES ${MAN1_OUTPUT_GZ} DESTINATION usr/share/man/man1 COMPONENT docs-man)
add_dependencies(docs-man docs-man1)
