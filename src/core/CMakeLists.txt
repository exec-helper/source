set(LIBRARY_NAME ${PROJECT_NAME}-core)

find_package(Boost REQUIRED COMPONENTS program_options)

set(SRCS
        src/pattern.cpp
        src/optionDescriptions.cpp
        src/task.cpp
        src/immediateExecutor.cpp
        src/reportingExecutor.cpp
        src/sequentialTaskRunner.cpp
        src/patterns.cpp
        src/patternsHandler.cpp
        src/execHelperOptions.cpp
        src/posixShell.cpp
        src/argv.cpp
        src/envp.cpp
   )

set(LIBRARY_INCLUDES
        ${${PROJECT_NAME}-yaml_INCLUDE_DIRS}
        ${${PROJECT_NAME}-config_INCLUDE_DIRS}
        ${${PROJECT_NAME}-log_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
    )

include_directories(
        include/core
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} CACHE INTERNAL ${LIBRARY_NAME}_INCLUDE_DIRS)

set(${LIBRARY_NAME}_LIBRARY_DIRS 
        ${${PROJECT_NAME}-yaml_LIBRARY_DIRS}
        ${${PROJECT_NAME}-config_LIBRARY_DIRS}
        ${${PROJECT_NAME}-log_LIBRARY_DIRS}
        ${Boost_LIBRARY_DIRS}
        CACHE INTERNAL ${LIBRARY_NAME}_LIBRARY_DIRS)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} 
        ${${PROJECT_NAME}-yaml_LIBRARIES}
        ${${PROJECT_NAME}-config_LIBRARIES}
        ${${PROJECT_NAME}-log_LIBRARIES}
        ${Boost_LIBRARIES}
        CACHE INTERNAL ${LIBRARY_NAME}_LIBRARIES)
