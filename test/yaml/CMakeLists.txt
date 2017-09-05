set(EXE_NAME ${PROJECT_NAME}-yaml-unittest)

set(SRCS    src/yamlTest.cpp
   )

set(DEPENDENCIES
        yaml
        test-utils
        unittest
    )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
