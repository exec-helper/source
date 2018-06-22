include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(BUILD_USAGE_MAN_DOCUMENTATION "Create and install the MAN usage documentation (requires Sphinx)" ON "BUILD_USAGE_DOCUMENTATION" OFF)
CMAKE_DEPENDENT_OPTION(BUILD_USAGE_HTML_DOCUMENTATION "Create and install the HTML usage documentation (requires Sphinx)" ON "BUILD_USAGE_DOCUMENTATION" OFF)

find_package(Sphinx REQUIRED)

if(BUILD_USAGE_HTML_DOCUMENTATION)
    include(html.CMakeLists.txt)
endif()

if(BUILD_USAGE_MAN_DOCUMENTATION)
    include(man.CMakeLists.txt)
endif()

