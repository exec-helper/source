set(EXE_NAME exec-helper)

set(SRCS
        exec-helper.cpp
   )

set(DEPENDENCIES
        dl
        boost-filesystem
        boost-optional
        log
        core
        commander
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

set(SHORT_EXE_NAME eh)
add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${EXE_NAME} ${SHORT_EXE_NAME}
        MAIN_DEPENDENCY ${EXE_NAME}
        COMMENT "Linking ${SHORT_EXE_NAME} -> ${EXE_NAME}"
    )

install(TARGETS ${EXE_NAME}
        DESTINATION ${BIN_DIRECTORY}
        COMPONENT runtime
       )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SHORT_EXE_NAME}
        DESTINATION ${BIN_DIRECTORY}
        COMPONENT runtime
       )
