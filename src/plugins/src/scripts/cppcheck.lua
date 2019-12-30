task:add_args({'cppcheck'})
task:add_args({"--enable=" .. table.concat(list(config['enable-checks']) or {'all'}, ',')})
task:add_args(get_verbose('--verbose'))
task:add_args({'-j', one(config['jobs']) or jobs})
task:add_args(get_commandline())

task:add_args(list(config['src-dir']) or {'.'})

register_task(task)
