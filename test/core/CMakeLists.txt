include(generators.CMakeLists.txt)

set(EXE_NAME ${PROJECT_NAME}-core-unittest)

set(SRCS
        src/permutationIteratorTest.cpp
        src/posixShellTest.cpp
        src/taskTest.cpp
        src/immediateExecutorTest.cpp
   )

set(DEPENDENCIES
        core-generators
        boost-program-options
        filesystem
        core
        test-utils
        unittest
        rpcheck
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR} COMPONENT core)
