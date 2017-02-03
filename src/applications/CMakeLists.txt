set(EXE_NAME exec-helper)

set(SRCS
        exec-helper.cpp
   )

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

install(TARGETS ${EXE_NAME} DESTINATION ${BIN_DIRECTORY})
