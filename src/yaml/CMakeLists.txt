set(LIBRARY_NAME ${PROJECT_NAME}-yaml)

set(SRCS
    src/yamlWrapper.cpp
    src/yaml.cpp
   )

set(LIBRARY_INCLUDES
        ${YAML_INCLUDE_DIRS}
        ${${PROJECT_NAME}-config_INCLUDE_DIRS}
        ${${PROJECT_NAME}-log_INCLUDE_DIRS}
    )

include_directories(
        include/yaml
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

if(NOT ${USE_SYSTEM_YAML_CPP})
    add_dependencies(${LIBRARY_NAME} Yaml-cpp)
endif(NOT ${USE_SYSTEM_YAML_CPP})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} PARENT_SCOPE)
set(${LIBRARY_NAME}_LIBRARY_DIRS
        ${${PROJECT_NAME}-config_LIBRARY_DIRS} 
        ${${PROJECT_NAME}-log_LIBRARY_DIRS} 
        ${YAML_LIBRARY_DIRS}
    PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME}
        ${${PROJECT_NAME}-config_LIBRARIES} 
        ${${PROJECT_NAME}-log_LIBRARIES} 
        ${YAML_LIBRARIES}
    PARENT_SCOPE)
