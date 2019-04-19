set(LIB_NAME log-generators)
add_library(${LIB_NAME} INTERFACE)
target_include_directories(${LIB_NAME} INTERFACE include)

set(EXE_NAME ${PROJECT_NAME}-log-unittest)

set(SRCS    src/logTest.cpp
   )

set(DEPENDENCIES
        boost-log
        log
        core
        rpcheck
        unittest
        log-generators
   )

add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${DEPENDENCIES})
target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENCIES})
add_test(${EXE_NAME} ${EXE_NAME})

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
