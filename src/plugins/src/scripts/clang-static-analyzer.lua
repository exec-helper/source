task:add_args({'scan-build'})
task:add_args(get_verbose('-v'))
task:add_args(get_commandline())

local build_commands = list(config['build-command'])

if type(build_commands) == 'nil' then
    input_error('Clang-static-analyzer: one must define at least one build command')
end

if type(next(build_commands)) == 'nil' then
    user_feedback_error('Clang-static-analyzer: one must define at least one build command')
    input_error('Clang-static-analyzer: one must define at least one build command')
end

run_target(task, build_commands)
