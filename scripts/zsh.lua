local cmd = one(config['command'])
if type(cmd) == 'nil' then
    input_error('Bash: error: you must define a command!')
end

task:add_args({'zsh', '-c', cmd})
task:add_args(get_commandline())

register_task(task)
