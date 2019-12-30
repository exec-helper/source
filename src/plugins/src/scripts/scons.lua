task:add_args({'scons'})
task:add_args({'--directory', one(config['build-dir']) or '.'})
task:add_args(get_verbose('--debug=explain'))
task:add_args({'--jobs', one(config['jobs']) or jobs})
task:add_args(get_commandline())

register_task(task)
