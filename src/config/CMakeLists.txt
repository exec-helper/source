set(MODULE_NAME config)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

find_package(Boost REQUIRED COMPONENTS filesystem)

set(SRCS    src/settingsNode.cpp
            src/configFileSearcher.cpp
)

set(DEPENDENCIES
        log
   )

set(LIBRARY_INCLUDES
    ${Boost_INCLUDE_DIRS}
)

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/config)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${LIBRARY_INCLUDES} ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES} ${Boost_LIBRARIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
