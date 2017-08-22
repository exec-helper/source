set(EXE_NAME exec-helper)

set(SRCS
        exec-helper.cpp
   )

if(${USE_SYSTEM_GSL})
    find_package(gsl REQUIRED)
endif()

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-commander_INCLUDE_DIRS}
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-log_INCLUDE_DIRS}
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-commander_LIBRARIES}
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-log_LIBRARIES}
    )

link_directories(${EXE_NAME}
        ${${PROJECT_NAME}-commander_LIBRARY_DIRS}
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-log_LIBRARY_DIRS}
    )

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

if(NOT ${USE_SYSTEM_GSL})
   add_dependencies(${EXE_NAME} gsl)
endif()


