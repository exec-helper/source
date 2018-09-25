set(EXE_NAME exec-helper)

set(SRCS
        exec-helper.cpp
   )

set(DEPENDENCIES
        dl
        filesystem
        boost-optional
        log
        core
        commander
    )

set(VERSION_FILE version.h)
set(BINARY_NAME ${PROJECT_NAME})
set(VERSION "DEVELOPER-VERSION" CACHE STRING "Set the version to show in the version message")
set(COPYRIGHT "COPYRIGHT (c)" CACHE STRING "Set the copyright message to show in the version message")
configure_file(${VERSION_FILE}.in ${VERSION_FILE})
set(VERSION_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXE_NAME} ${SRCS} ${VERSION_FILE})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES} ${VERSION_INCLUDE_DIR})
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
