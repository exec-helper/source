set(MODULE_NAME config)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS    src/settingsNode.cpp
            src/configFileSearcher.cpp
            src/logger.cpp
            src/fleetingOptions.cpp
            src/config.cpp
            src/pattern.cpp
            src/patternsHandler.cpp
            src/optionDescriptions.cpp
            src/argv.cpp
            src/envp.cpp
            src/cast.cpp
)

set(DEPENDENCIES
        boost-optional
        boost-filesystem
        log
        yaml
   )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/config)
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
