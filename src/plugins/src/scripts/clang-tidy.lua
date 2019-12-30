task:add_args({'clang-tidy'})
task:add_args({'-p', one(config['build-dir']) or '.'})

checks = list(config['checks'])
if checks then
    task:add_args({'-checks=' .. table.concat(checks, ',')})
end

warning_as_errors = list(config['warning-as-errors'])
if warning_as_errors then
    if warning_as_errors[1] == 'auto' then
        -- Mirror checks
        task:add_args({'-warning-as-errors=' .. table.concat(checks, ',')})
    else
        task:add_args({'-warning-as-errors=' .. table.concat(warning_as_errors, ',')})
    end
end

task:add_args(get_commandline())
task:add_args(list(config['sources']) or {"*.cpp"})

register_task(task)
