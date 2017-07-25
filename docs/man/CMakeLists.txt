option(BUILD_MAN_DOCUMENTATION "Create and install the MAN API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(FILEPATH ${ARGN})
       LIST(APPEND listVar "${prefix}/${FILEPATH}")
   ENDFOREACH(FILEPATH)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

FUNCTION(STRIP_PATHS var)
   SET(listVar "")
   FOREACH(FILEPATH ${ARGN})
       get_filename_component(STRIPPED ${FILEPATH} NAME)
      LIST(APPEND listVar ${STRIPPED})
  ENDFOREACH(FILEPATH)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(STRIP_PATHS)

if(BUILD_MAN_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required to build the documentation.")
    endif()

    if(CMAKE_VERBOSE_MAKEFILE)
        set(DOXYGEN_QUIET NO)
    else()
        set(DOXYGEN_QUIET YES)
    endif()

    set(MAN1_SOURCES    src/applications/exec-helper.md)
    set(MAN1_SOURCES_INTEGRATION    test/integration/exec-helper-integration-test.md)
    set(MAN1_EXTENSION .1)

    set(MAN5_SOURCES    src/config/docs/exec-helper-config.md
                    src/config/docs/exec-helper-config-patterns.md
                    src/config/docs/exec-helper-config-environment.md
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

    find_package(Gzip REQUIRED)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is required to build the documentation.")
    endif()

    add_custom_target(docs-man COMMENT "Building man-page API documentation" )
    add_dependencies(docs docs-man)

    add_subdirectory(man1)
    add_subdirectory(man1-integration)
    add_subdirectory(man5)
endif()
