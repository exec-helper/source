set(LIBRARY_NAME ${PROJECT_NAME}-test-utils)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
if(${USE_SYSTEM_GSL})
    find_package(gsl REQUIRED)
endif()

set(SRCS
        src/utils.cpp
        src/combinationHelpers.cpp
        src/tmpFile.cpp
   )

set(LIBRARY_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../stubs
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-plugins_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIR}
    )

include_directories(${LIBRARY_NAME}
        include/utils
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

if(NOT ${USE_SYSTEM_GSL})
    add_dependencies(${LIBRARY_NAME} gsl)
endif()

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} CACHE INTERNAL ${LIBRARY_NAME}_INCLUDE_DIRS)

set(${LIBRARY_NAME}_LIBRARY_DIRS 
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-plugins_LIBRARY_DIRS}
        ${Boost_LIBRARY_DIRS}
    CACHE INTERNAL ${LIBRARY_NAME}_LIBRARY_DIRS)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} 
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-plugins_LIBRARIES}
        ${Boost_LIBRARIES}
    CACHE INTERNAL ${LIBRARY_NAME}_LIBRARIES)

install(TARGETS ${EXE_NAME} DESTINATION ${UNITTEST_BIN_DIR})
