local exec = one(config['exec']) or 'pmd'
local tool = one(config['tool']) or 'cpd'
task:add_args({exec .. '-' .. tool})

local language = one(config['language'])
if language then
    task:add_args({'--language', language})
end

task:add_args(get_verbose('-verbose'))

if tool == 'cpd' then
    task:add_args({'--minimum-tokens', one(config['minimum-tokens']) or '80'})

    local files = list(config['files'])
    if files then
        for _,file in ipairs(files) do
            task:add_args({'--files', file})
        end
    end
end

task:add_args(get_commandline())

register_task(task)
