pattern_key = one(config['patterns'])

if pattern_key == nil then
    user_feedback_error('Undefined selector pattern: you must define a pattern to select using the "patterns" keyword.')    
    error('Undefined selector pattern: you must define a pattern to select using the "patterns" keyword.')
end

values = patterns[pattern_key]

if values == nil then
    user_feedback_error('Selector: Inactive or unknown select pattern used')
    error('Selector: Inactive or unknown select pattern used')
end

run_target(task, values)
