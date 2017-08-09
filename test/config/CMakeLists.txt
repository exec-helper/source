set(EXE_NAME ${PROJECT_NAME}-config-unittest)

find_package(Boost REQUIRED COMPONENTS filesystem)

set(SRCS    src/settingsNodeTest.cpp
            src/configFileSearcherTest.cpp
   )

add_executable(${EXE_NAME} ${SRCS})

include_directories(${EXE_NAME}
        ${${PROJECT_NAME}-config_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${UNITTEST_INCLUDE_DIRS} 
    )

link_directories(
    ${${PROJECT_NAME}-config_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS}
    ${UNITTEST_LIBRARY_DIRS}
)

target_link_libraries(${EXE_NAME}
        ${${PROJECT_NAME}-config_LIBRARIES}
        ${Boost_LIBRARIES}
        ${UNITTEST_LIBRARIES}
    )

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
