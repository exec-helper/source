set(EXE_NAME ${PROJECT_NAME}-core-unittest)

set(SRCS
        src/patternTest.cpp
        src/permutationIteratorTest.cpp
        src/posixShellTest.cpp
        src/sequentialTaskRunnerTest.cpp
        src/taskTest.cpp
        src/patternHandlerTest.cpp
        src/execHelperOptionsTest.cpp
        src/immediateExecutorTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-test-utils_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-test-utils_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

link_directories(${EXE_NAME}
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-test-utils_LIBRARY_DIRS}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
