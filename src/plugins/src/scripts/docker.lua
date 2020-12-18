task:add_args({"docker"})

local mode = one(config['mode']) or 'exec'
local interactive = '--interactive=' .. (one(config['interactive']) == 'yes' and 'true' or 'false')
local tty = '--tty=' .. (one(config['tty']) == 'yes' and 'true' or 'false')
task:add_args({mode, interactive, tty})

local privileged = one(config['privileged'])
if privileged == 'yes' then
    task:add_args({'--privileged'})
end

local user = one(config['user'])
if user then
    task:add_args({'--user="' .. user .. '"'})
end

local env = config['env']
if env then
    for key,value in pairs(config['env']) do
        task:add_args({'"--env=' .. key .. '=' .. value .. '"' })
    end
end

task:add_args(get_commandline())

if mode == 'run' then
    local volumes = list(config['volumes'])
    if volumes then
        for _,volume in ipairs(volumes) do
            task:add_args({'"--volume=' .. volume .. '"' })
        end
    end

    local image = one(config['image'])
    if image == nil then
        user_feedback_error('You must define an image when you define the docker plugin in "run" mode!')
        input_error('You must define an image when you define the docker plugin in "run" mode!')
    end

    task:add_args({image})
end

if mode == 'exec' then
    local container = one(config['container'])

    if container == nil then
        user_feedback_error('You must define an container when you define the docker plugin in "exec" mode!')
        input_error('You must define an container when you define the docker plugin in "exec" mode!')
    end

    task:add_args({container})
end

local targets = list(config['targets'])
if targets then
    register_tasks(run_target(task, targets))
else
    register_task(task)
end
