set(EXE_NAME ${PROJECT_NAME}-commander-unittest)

set(SRCS
        src/commanderTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-commander_INCLUDE_DIRS}
        ${${PROJECT_NAME}-test-utils_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-commander_LIBRARIES}
        ${${PROJECT_NAME}-test-utils_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

link_directories(${EXE_NAME}
        ${${PROJECT_NAME}-commander_LIBRARY_DIRS}
        ${${PROJECT_NAME}-test-utils_LIBRARY_DIRS}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
