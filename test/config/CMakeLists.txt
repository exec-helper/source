set(EXE_NAME ${PROJECT_NAME}-config-unittest)

set(SRCS    src/settingsNodeTest.cpp
            src/configFileSearcherTest.cpp
   )

set(DEPENDENCIES
        boost-program-options
        boost-filesystem
        config
        unittest
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
