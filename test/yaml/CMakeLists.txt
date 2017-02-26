set(EXE_NAME ${PROJECT_NAME}-yaml-unittest)

set(SRCS    src/yamlTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-yaml_INCLUDE_DIRS}
        ${${PROJECT_NAME}-test-utils_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-yaml_LIBRARIES}
        ${${PROJECT_NAME}-test-utils_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

link_directories(${EXE_NAME}
        ${${PROJECT_NAME}-yaml_LIBRARY_DIRS}
        ${${PROJECT_NAME}-test-utils_LIBRARY_DIRS}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
