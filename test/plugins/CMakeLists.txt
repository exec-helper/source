set(EXE_NAME ${PROJECT_NAME}-plugins-unittest)

set(SRCS
        src/genericPluginTest.cpp
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
        src/cmakeTest.cpp
        src/dockerTest.cpp
   )

set(DEPENDENCIES
        plugins
        test-utils
        unittest
        log-generators
        config-generators
        core-generators
        rpcheck
    )


add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES} include/plugins)
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
