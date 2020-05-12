set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Gzip REQUIRED)

set(MAN_EXTENSION ${MAN1_EXTENSION})
set(REFERENCED_PAGES ${MAN5_SOURCES})      # Reference these pages in order to avoid Doxygen complaining about not finding them. Do not install them though.

# Create the input paths for the Doxyfile
PREPEND(MAN1_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN1_SOURCES} ${REFERENCED_PAGES})
string (REPLACE ";" " " MAN_SOURCES "${MAN1_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create temporary intermediate output paths
STRIP_PATHS(MAN1_STRIPPED ${MAN1_SOURCES})
list(TRANSFORM MAN1_STRIPPED REPLACE "\.md$" "${MAN1_EXTENSION}" OUTPUT_VARIABLE MAN1_OUTPUT_TMP)

# Create the gzipped output paths
list(TRANSFORM MAN1_STRIPPED REPLACE "\.md$" "${MAN1_EXTENSION}.gz" OUTPUT_VARIABLE MAN1_OUTPUT_GZ)
PREPEND(MAN1_OUTPUT_FULL_PATH ${CMAKE_CURRENT_BINARY_DIR} ${MAN1_OUTPUT_GZ})

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_target(docs-man1
    ${DOXYGEN_EXECUTABLE} ${doxyfile}
    COMMAND ${Gzip} --force ${MAN1_OUTPUT_TMP}
    SOURCES ${doxyfile_in}
    BYPRODUCTS ${MAN1_OUTPUT_GZ}
    COMMENT "Generating MAN1 API documentation..."
    VERBATIM
)

install(FILES ${MAN1_OUTPUT_FULL_PATH} DESTINATION share/man/man1 COMPONENT docs-man)
install(FILES ${MAN1_OUTPUT_FULL_PATH} DESTINATION share/man/man1 RENAME eh.1.gz COMPONENT docs-man)

add_dependencies(docs-man docs-man1)
