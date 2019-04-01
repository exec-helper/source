set(LIB_NAME config-generators)
add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE include)

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
            src/pathManipulationTest.cpp
   )

set(DEPENDENCIES
        filesystem
        boost-program-options
        config
        unittest
        rpcheck
        test-utils
        log-generators
        config-generators
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES} include)
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
