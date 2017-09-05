set(EXE_NAME ${PROJECT_NAME}-config-unittest)

find_package(Boost REQUIRED COMPONENTS filesystem)

set(SRCS    src/settingsNodeTest.cpp
            src/configFileSearcherTest.cpp
   )

set(DEPENDENCIES
        config
        unittest
        ${Boost_INCLUDE_DIRS}
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
