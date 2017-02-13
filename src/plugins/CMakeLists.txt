set(LIBRARY_NAME ${PROJECT_NAME}-plugins)

set(SRCS
        src/bootstrap.cpp
        src/pmd.cpp
        src/selector.cpp
        src/make.cpp
        src/pluginUtils.cpp
        src/memory.cpp
        src/commandPlugin.cpp
        src/cppcheck.cpp
        src/valgrind.cpp
        src/executePlugin.cpp
        src/plugin.cpp
        src/scons.cpp
        src/commandLineCommand.cpp
        src/buildPlugin.cpp
        src/clangStaticAnalyzer.cpp
        src/lcov.cpp
   )

set(LIBRARY_INCLUDES
        ${${PROJECT_NAME}-core_INCLUDE_DIRS}
        ${${PROJECT_NAME}-config_INCLUDE_DIRS}
        ${${PROJECT_NAME}-log_INCLUDE_DIRS}
    )

include_directories(
        include/plugins
        ${LIBRARY_INCLUDES}
    )

add_library(${LIBRARY_NAME} ${SRCS})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})

set(${LIBRARY_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBRARY_INCLUDES} PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARY_DIRS 
        ${${PROJECT_NAME}-core_LIBRARY_DIRS}
        ${${PROJECT_NAME}-config_LIBRARY_DIRS}
        ${${PROJECT_NAME}-log_LIBRARY_DIRS}
    PARENT_SCOPE)

set(${LIBRARY_NAME}_LIBRARIES ${LIBRARY_NAME} 
        ${${PROJECT_NAME}-core_LIBRARIES}
        ${${PROJECT_NAME}-config_LIBRARIES}
        ${${PROJECT_NAME}-log_LIBRARIES}
    PARENT_SCOPE)
