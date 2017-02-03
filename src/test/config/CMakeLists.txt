set(EXE_NAME ${PROJECT_NAME}-config-unittest)

set(SRCS    src/settingsNodeTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-config_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-config_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
