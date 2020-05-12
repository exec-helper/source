set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Gzip REQUIRED)

set(MAN_EXTENSION ${MAN5_EXTENSION})
set(REFERENCED_PAGES ${MAN1_SOURCES})      # Reference these pages in order to avoid Doxygen complaining about not finding them. Do not install them though.

# Create the input paths for the Doxyfile
PREPEND(MAN5_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN5_SOURCES} ${REFERENCED_PAGES})
string (REPLACE ";" " " MAN_SOURCES "${MAN5_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create temporary intermediate output paths
#string(REPLACE ".md" "${MAN5_EXTENSION}" MAN5_OUTPUT_TMP ${MAN5_STRIPPED})
STRIP_PATHS(MAN5_STRIPPED ${MAN5_SOURCES})
list(TRANSFORM MAN5_STRIPPED REPLACE "\.md$" ".5" OUTPUT_VARIABLE MAN5_OUTPUT_TMP)

# Create the gzipped output paths
list(TRANSFORM MAN5_STRIPPED REPLACE "\.md$" ".5.gz" OUTPUT_VARIABLE MAN5_OUTPUT_GZ)
PREPEND(MAN5_OUTPUT_FULL_PATH ${CMAKE_CURRENT_BINARY_DIR} ${MAN5_OUTPUT_GZ})

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(docs-man5
    ${DOXYGEN_EXECUTABLE} ${doxyfile}
    COMMAND ${Gzip} --force ${MAN5_OUTPUT_TMP}
    #    COMMAND ${Gzip} --force $<JOIN:${MAN5_OUTPUT_TMP},blaat>
    SOURCES ${doxyfile_in}
    BYPRODUCTS ${MAN5_OUTPUT_GZ}
    COMMENT "Generating MAN5 API documentation..."
    VERBATIM
)

install(FILES ${MAN5_OUTPUT_FULL_PATH} DESTINATION share/man/man5 COMPONENT docs-man)

add_dependencies(docs-man docs-man5)
