set(LIBRARY_NAME ${PROJECT_NAME}-config)

find_package(Boost REQUIRED COMPONENTS filesystem)
if(${USE_SYSTEM_GSL})
    find_package(gsl REQUIRED)
endif()

set(SRCS    src/settingsNode.cpp
            src/configFileSearcher.cpp
)

set(LIBRARY_INCLUDES
        ${Boost_INCLUDE_DIRS}
)

include_directories(
    include/config
    ${GSL_INCLUDE_DIRS}
    ${LIBRARY_INCLUDES}
)

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

if(NOT ${USE_SYSTEM_GSL})
    add_dependencies(${LIBRARY_NAME} gsl)
endif()

set(${LIBRARY_NAME}_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL ${LIBRARY_NAME}_INCLUDE_DIRS)

set(${LIBRARY_NAME}_LIBRARIES
    ${LIBRARY_NAME}
    ${GSL_LIBRARIES}
    ${Boost_LIBRARIES}
    CACHE INTERNAL ${LIBRARY_NAME}_LIBRARIES)

set(${LIBRARY_NAME}_LIBRARY_DIRS
    ${GSL_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS}
    CACHE INTERNAL ${LIBRARY_NAME}_LIBRARY_DIRS)
