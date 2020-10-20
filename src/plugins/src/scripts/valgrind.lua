task:add_args({'valgrind'})

local targets = list(config['run-command'])
if targets == nil then
    user_feedback_error('Valgrind script: You must configure at least one run-command')
    input_error('Valgrind script: You must configure at least one run-command')
end

local tool = one(config['tool'])
if tool then
    task:add_args({'--tool=' .. tool})
end

task:add_args(get_verbose('--verbose'))
task:add_args(get_commandline())

run_target(task, targets)
