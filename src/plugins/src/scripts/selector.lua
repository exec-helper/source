pattern_key = one(config['patterns'])

if pattern_key == nil then
    input_error('Undefined selector pattern: you must define a pattern to select using the "patterns" keyword.')
end

values = patterns[pattern_key]

if values == nil then
    input_error('Selector: Inactive or unknown select pattern used')
end

run_target(task, values)
