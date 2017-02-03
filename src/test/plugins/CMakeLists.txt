set(EXE_NAME ${PROJECT_NAME}-plugins-unittest)

set(SRCS
        src/buildPluginTest.cpp
        src/clangStaticAnalyzerTest.cpp
        src/pmd.cpp
        src/commandLineCommandTest.cpp
        src/cppcheckTest.cpp
        src/pluginUtilsTest.cpp
        src/bootstrapTest.cpp
        src/executePluginTest.cpp
        src/valgrindTest.cpp
        src/selectorTest.cpp
        src/sconsTest.cpp
        src/makeTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-plugins_INCLUDE_DIRS}
        ${${PROJECT_NAME}-test-utils_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-plugins_LIBRARIES}
        ${${PROJECT_NAME}-test-utils_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

link_directories(${EXE_NAME}
        ${${PROJECT_NAME}-plugins_LIBRARY_DIRS}
        ${${PROJECT_NAME}-test-utils_LIBRARY_DIRS}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
