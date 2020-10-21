local base_directory = one(config['base-directory']) or '.'
local directory = one(config['directory']) or '.'
local info_file = one(config['info-file']) or 'lcov-plugin.info'

local zero_counters = one(config['zero-counters'])
if zero_counters == 'yes' then
    local zero_task = task:copy()
    zero_task:add_args({'lcov', '--base-directory', base_directory, '--directory', directory, '--zerocounters'})
    zero_task:add_args(get_commandline())
    register_task(zero_task)
end

local targets = list(config['run-command'])
if targets == nil then
    user_feedback_error('Lcov script: You must configure at least one run-command')
    input_error('Lcov script: You must configure at least one run-command')
end

local run_task = task:copy()
run_target(run_task, targets)

local capture_task = task:copy()
capture_task:add_args({'lcov', '--base-directory', base_directory, '--directory', directory, '--capture', '--output', info_file})
capture_task:add_args(get_commandline())
register_task(capture_task)

local excludes = list(config['excludes'])
if excludes then
    local exclude_task = task:copy()
    exclude_task:add_args({'lcov', '--remove', info_file})
    for _,exclude in ipairs(excludes) do
        exclude_task:add_args({"'" .. exclude .. "'"})
    end
    exclude_task:add_args({'--output-file', info_file})
    exclude_task:add_args(get_commandline())
    register_task(exclude_task)
end

local gen_html = one(config['gen-html'])
if gen_html == 'yes' then
    local output = one(config['gen-html-output']) or '.'
    local title = one(config['gen-html-title']) or 'Hello'
    local command_line = list(config['gen-html-command-line']) or {}

    local gen_html_task = task:copy()
    gen_html_task:add_args({"genhtml", '--output-directory', output, '--title', title})
    gen_html_task:add_args(command_line)
    gen_html_task:add_args({info_file})
    register_task(gen_html_task)
end
