set(CHANGELOG_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/../.gitchangelog.rc)
set(CHANGELOG_OUTPUT   ${CMAKE_CURRENT_BINARY_DIR}/exec-helper.changelog)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(Gitchangelog)

if(Gitchangelog_FOUND)
    add_custom_command(OUTPUT ${CHANGELOG_OUTPUT}
        COMMAND ${Gitchangelog} > ${CHANGELOG_OUTPUT}
        MAIN_DEPENDENCY ${CHANGELOG_SOURCES}
        VERBATIM)

    add_custom_target(changelog
        SOURCES ${CHANGELOG_OUTPUT}
        COMMENT "Generating changelog"
        VERBATIM)

    install(FILES ${CHANGELOG_OUTPUT} DESTINATION share/doc/exec-helper COMPONENT changelog)
    add_custom_target(all-changelog ALL)
    add_dependencies(all-changelog changelog)
else()
    MESSAGE(STATUS "Disabled 'changelog' target")
endif()
