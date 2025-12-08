task:add_args({one(config['filename']) or 'bootstrap.sh'})
task:add_args(get_commandline())

task:register()
