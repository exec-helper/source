targets = list(config['targets'])

if targets == nil then
    input_error('Undefined selector target: you must define at least one target to select using the "targets" keyword.')
end

register_tasks(run_target(task, targets))
