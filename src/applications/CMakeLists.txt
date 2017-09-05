set(EXE_NAME exec-helper)

set(SRCS
        exec-helper.cpp
   )

set(DEPENDENCIES
        boost-filesystem
        boost-optional
        log
        core
        commander
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

install(TARGETS ${EXE_NAME}
        DESTINATION ${BIN_DIRECTORY}
        COMPONENT runtime
       )

# Create eh symlink if supported by the OS
set(SHORT_EXE_NAME eh)
install(CODE "MESSAGE(\"-- Linking ${CMAKE_INSTALL_PREFIX}/${BIN_DIRECTORY}/${SHORT_EXE_NAME} -> ${EXE_NAME}\")
              execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${EXE_NAME} ${CMAKE_INSTALL_PREFIX}/${BIN_DIRECTORY}/${SHORT_EXE_NAME})"
        COMPONENT runtime
       )
