set(EXE_NAME ${PROJECT_NAME}-config-unittest)

set(SRCS    src/settingsNodeTest.cpp
            src/configFileSearcherTest.cpp
            src/patternTest.cpp
            src/patternHandlerTest.cpp
            src/configTest.cpp
            src/fleetingOptionsTest.cpp
            src/commandLineOptionsTest.cpp
            src/argvTest.cpp
            src/envpTest.cpp
   )

set(DEPENDENCIES
        boost-program-options
        boost-filesystem
        config
        unittest
        test-utils
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
