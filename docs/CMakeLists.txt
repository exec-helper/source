find_package(Doxygen)
add_custom_target(docs ALL)

add_subdirectory(html)
add_subdirectory(xml)
add_subdirectory(man)
