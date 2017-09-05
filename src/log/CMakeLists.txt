set(MODULE_NAME log)
set(LIBRARY_NAME ${PROJECT_NAME}-${MODULE_NAME})

set(DEPENDENCIES
        gsl
   )

add_library(${LIBRARY_NAME} INTERFACE)
target_include_directories(${LIBRARY_NAME} INTERFACE include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} INTERFACE ${DEPENDENCIES})

add_library(${MODULE_NAME} ALIAS ${LIBRARY_NAME})
