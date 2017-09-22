set(MODULE_NAME test-base-utils)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(SRCS
        src/executionContent.cpp
        src/tmpFile.cpp
        src/configFileWriter.cpp
        src/generateRandom.cpp
        src/executionHandler.cpp
   )

set(DEPENDENCIES
        boost-filesystem
        yaml-3rd
    )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PRIVATE include/base-utils)
target_include_directories(${LIBRARY_NAME} PUBLIC ../stubs include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
