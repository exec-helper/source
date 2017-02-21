set(LIBRARY_NAME ${PROJECT_NAME}-test-utils)

set(SRCS
        src/utils.cpp
   )

set(LIBRARY_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../stubs
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-plugins_INCLUDE_DIRS}
    )

include_directories(
        include/utils
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARY_DIRS 
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-plugins_LIBRARY_DIRS}
    PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} 
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-plugins_LIBRARIES}
    PARENT_SCOPE)

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
