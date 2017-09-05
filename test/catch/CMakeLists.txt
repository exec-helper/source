set(LIBRARY_NAME ${PROJECT_NAME}-catch-main)

set(SRCS    catch.cpp
   )

set(DEPENDENCIES
        catch
    )

add_library(${LIBRARY_NAME} ${SRCS})
target_include_directories(${LIBRARY_NAME} PUBLIC include ${DEPENDENCIES})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})

add_library(unittest ALIAS ${LIBRARY_NAME})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIB_DIRECTORY})
