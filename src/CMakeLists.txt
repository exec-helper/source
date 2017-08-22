add_subdirectory(log)
add_subdirectory(config)
add_dependencies(${PROJECT_NAME}-config ${${PROJECT_NAME}-log_DEPENDENCIES})    # Since log does not create any files itself, depend on a another target that is low enough in the dependency tree

add_subdirectory(yaml)
add_subdirectory(core)
add_subdirectory(plugins)
add_subdirectory(commander)
add_subdirectory(applications)
