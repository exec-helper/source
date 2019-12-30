task:add_args({'ninja'})
task:add_args({'-C', one(config['build-dir']) or '.'})
task:add_args(get_verbose('--verbose'))
task:add_args({'-j', one(config['jobs']) or jobs})
task:add_args(get_commandline())

register_task(task)
