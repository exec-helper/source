task:add_args({'cmake'})

local mode = one(config['mode']) or 'generate'

if mode ~= 'generate' and mode ~= 'build' and mode ~= 'install' then
    user_feedback_error('You must define a valid mode! Options are: generate, build or install.')
    input_error('You must define a valid mode!')
end

if mode == 'generate' then
    task:add_args({'-S', one(config['source-dir']) or '.'})
    task:add_args({'-B', one(config['build-dir']) or '.'})

    local generator = one(config['generator'])
    if generator then
        task:add_args({'-G', generator})
    end

    local defines = config['defines']
    if defines then
        for k, v in pairs(defines) do
            task:add_args({'-D', '"' .. k .. '=' .. v .. '"'})
        end
    end

    task:add_args(get_verbose('--log-level=VERBOSE'))
end

if mode == 'build' then
    task:add_args({'--build', one(config['build-dir']) or '.'})

    local target = one(config['target'])
    if target then
        task:add_args({'--target', target})
    end

    local configuration = one(config['configuration'])
    if configuration then
        task:add_args({'--config', configuration})
    end

    task:add_args({'--parallel', one(config['jobs']) or jobs})
    task:add_args(get_verbose('--verbose'))
end

if mode == 'install' then
    task:add_args({'--install', one(config['build-dir']) or '.'})

    local configuration = one(config['configuration'])
    if configuration then
        task:add_args({'--config', configuration})
    end

    local prefix = one(config['prefix'])
    if prefix then
        task:add_args({'--prefix', prefix})
    end

    local component = one(config['component'])
    if component then
        task:add_args({'--component', component})
    end

    task:add_args(get_verbose('--verbose'))
end

task:add_args(get_commandline())

register_task(task)
