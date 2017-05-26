set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../Doxyfile.in)
set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(MAN5_SOURCES    src/config/docs/exec-helper-config.md
                    src/plugins/docs/exec-helper-plugins-pmd.md
                    src/plugins/docs/exec-helper-plugins-valgrind.md
                    src/plugins/docs/exec-helper-plugins-execute-plugin.md
                    src/plugins/docs/exec-helper-plugins-selector.md
                    src/plugins/docs/exec-helper-plugins-cppcheck.md
                    src/plugins/docs/exec-helper-plugins.md
                    src/plugins/docs/exec-helper-plugins-command-plugin.md
                    src/plugins/docs/exec-helper-plugins-bootstrap.md
                    src/plugins/docs/exec-helper-plugins-memory.md
                    src/plugins/docs/exec-helper-plugins-scons.md
                    src/plugins/docs/exec-helper-plugins-make.md
                    src/plugins/docs/exec-helper-plugins-clang-tidy.md
                    src/plugins/docs/exec-helper-plugins-command-line-command.md
                    src/plugins/docs/exec-helper-plugins-clang-static-analyzer.md
                    src/plugins/docs/exec-helper-plugins-lcov.md 
    )
set(MAN5_EXTENSION .5)
set(MAN_EXTENSION ${MAN5_EXTENSION})

# Create the input paths for the Doxyfile
PREPEND(MAN5_SOURCES_FULL_PATH ${PROJECT_SOURCE_DIR} ${MAN5_SOURCES})
string (REPLACE ";" " " MAN_SOURCES "${MAN5_SOURCES_FULL_PATH}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

# Create the output paths
STRIP_PATHS(MAN5_STRIPPED ${MAN5_SOURCES})
PREPEND(MAN5_OUTPUT_TMP ${CMAKE_CURRENT_BINARY_DIR} ${MAN5_STRIPPED})
string (REPLACE ".md" "${MAN5_EXTENSION}" MAN5_OUTPUT_PAGES "${MAN5_OUTPUT_TMP}")

# Create the gzipped output paths
string(REPLACE "${MAN5_EXTENSION}" "${MAN5_EXTENSION}.gz" MAN5_OUTPUT_GZ "${MAN5_OUTPUT_PAGES}")

# Set example paths for Doxygen
set(EXAMPLE_PATH_LIST ${PROJECT_SOURCE_DIR}/src/plugins/examples ${PROJECT_SOURCE_DIR}/src/plugins/examples)
string (REPLACE ";" " " EXAMPLE_PATH "${EXAMPLE_PATH_LIST}")    # CMAKE separates multiple values with a ';', while Doxygen uses ' '

configure_file(${doxyfile_in} ${doxyfile} @ONLY)

add_custom_command(OUTPUT ${MAN5_OUTPUT_PAGES}
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    MAIN_DEPENDENCY ${doxyfile_in}
    DEPENDS ${MAN5_SOURCES_FULL_PATH}
    VERBATIM)

FOREACH(FILE ${MAN5_OUTPUT_PAGES})
    add_custom_command(OUTPUT ${FILE}.gz
        COMMAND ${GZIP} ${FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${FILE}
        VERBATIM)
ENDFOREACH(FILE)

add_custom_target(docs-man5
    SOURCES ${MAN5_OUTPUT_GZ}
    COMMENT "Generating MAN5 API documentation..."
    VERBATIM)

install(FILES ${MAN5_OUTPUT_GZ} DESTINATION usr/share/man/man5 COMPONENT docs-man)

add_dependencies(docs-man docs-man5)
