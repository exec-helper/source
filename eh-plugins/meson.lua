task:add_args({'meson'})

local mode = one(config['mode']) or 'setup'

if mode ~= 'setup' and mode ~= 'compile' and mode ~= 'install' and mode ~= 'test' then
    input_error('You must define a valid mode! Options are: setup, compile, install or test.')
end

task:add_args({mode})

local build_dir = one(config['build-dir']) or '.'

-- Configure optional arguments
if mode == 'setup' then
    --if isdir(build_dir) then
        -- Build directory already exists. Let's reconfigure it
        --task:add_args({'--reconfigure'})
    --end

    local build_mode = one(config['build-type'])
    if build_mode then
        task:add_args({'--buildtype', build_mode})
    end

    local cross_file = one(config['cross-file'])
    if cross_file then
        task:add_args({'--cross-file', cross_file})
    end

    local prefix = one(config['prefix'])
    if prefix then
        task:add_args({'--prefix', prefix})
    end

    local options = config['options']
    if options then
        for k, v in pairs(options) do
            task:add_args({'-D', k .. '=' .. v})
        end
    end
end

if mode == 'compile' then
    task:add_args({'-C', build_dir})

    task:add_args({'--jobs', one(config['jobs']) or jobs})
end

if mode == 'install' then
    task:add_args({'-C', build_dir})
end

if mode == 'test' then
    task:add_args({'-C', build_dir})

    local suites = list(config['suite'])
    if suites then
        for _,suite in ipairs(suites) do
            task:add_args({'--suite', suite})
        end
    end

    local targets = list(config['targets'])
    if targets then
        task:add_args(targets)
    end
end

task:add_args(get_commandline())

-- Configure positional arguments
if mode == 'setup' then
    task:add_args({build_dir, one(config['source-dir']) or '.'})
end

register_task(task)
