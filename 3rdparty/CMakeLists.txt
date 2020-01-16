list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Define a target that downloads all required dependencies
add_custom_target(download)

include(stl.CMakeLists.txt)
include(boost.CMakeLists.txt)
include(yaml-cpp.CMakeLists.txt)
include(gsl.CMakeLists.txt)
include(luajit.CMakeLists.txt)
include(luawrapper.CMakeLists.txt)

if(${ENABLE_TESTING})
    include(catch.CMakeLists.txt)
    include(rapidcheck.CMakeLists.txt)
endif()
