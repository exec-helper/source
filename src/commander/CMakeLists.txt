set(LIBRARY_NAME ${PROJECT_NAME}-commander)

set(SRCS
    src/commander.cpp
   )

set(LIBRARY_INCLUDES
        ${${PROJECT_NAME}-plugins_INCLUDE_DIRS}
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-log_INCLUDE_DIRS}
    )

include_directories(
        include/commander
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARY_DIRS 
        ${${PROJECT_NAME}-plugins_LIBRARY_DIRS}
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-log_LIBRARY_DIRS}
    PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} 
        ${${PROJECT_NAME}-plugins_LIBRARIES}
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-log_LIBRARIES}
    PARENT_SCOPE)
