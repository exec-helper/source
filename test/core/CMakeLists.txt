set(EXE_NAME ${PROJECT_NAME}-core-unittest)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)

set(SRCS
        src/patternTest.cpp
        src/permutationIteratorTest.cpp
        src/posixShellTest.cpp
        src/taskTest.cpp
        src/patternHandlerTest.cpp
        src/execHelperOptionsTest.cpp
        src/immediateExecutorTest.cpp
   )

set(DEPENDENCIES
        core
        test-utils
        unittest
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES} ${Boost_INCLUDE_DIR})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES} ${Boost_LIBRARIES})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
