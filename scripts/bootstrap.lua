task:add_args({one(config['filename']) or 'bootstrap.sh'})
task:add_args(get_commandline())

register_task(task)
