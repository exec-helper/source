set(EXE_NAME ${PROJECT_NAME}-plugins-unittest)

set(SRCS
        src/buildPluginTest.cpp
        src/clangStaticAnalyzerTest.cpp
        src/clangTidyTest.cpp
        src/pmdTest.cpp
        src/commandLineCommandTest.cpp
        src/commandPluginTest.cpp
        src/cppcheckTest.cpp
        src/pluginUtilsTest.cpp
        src/bootstrapTest.cpp
        src/executePluginTest.cpp
        src/valgrindTest.cpp
        src/selectorTest.cpp
        src/sconsTest.cpp
        src/makeTest.cpp
        src/lcovTest.cpp
        src/ninjaTest.cpp
   )

set(DEPENDENCIES
        plugins
        test-utils
        unittest
    )


add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
