list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(stl.CMakeLists.txt)
include(boost.CMakeLists.txt)
include(yaml-cpp.CMakeLists.txt)
include(gsl.CMakeLists.txt)

if(${ENABLE_TESTING})
    include(catch.CMakeLists.txt)
    include(rapidcheck.CMakeLists.txt)
endif()
