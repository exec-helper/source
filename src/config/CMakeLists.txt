set(LIBRARY_NAME ${PROJECT_NAME}-config)

set(SRCS    src/settingsNode.cpp
   )

include_directories(include/config)

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} PARENT_SCOPE)
