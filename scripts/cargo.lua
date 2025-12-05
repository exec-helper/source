local command = one(config['command'])
if type(command) == 'nil' then
  input_error('You must define a command when using the cargo plugin!')
end

task:add_args({'cargo', command})

if command == 'build' or command == 'run' then
  local profile = one(config['profile'])
  if type(profile) ~= 'nil' then
    task:add_args({'--profile', profile})
  end

  local target = one(config['target'])
  if type(target) ~= 'nil' then
    task:add_args({'--target', target})
  end
end

if command == 'build' or command == 'run' or command == 'clippy' then
  if one(config['all-targets']) then
    task:add_args({'--all-targets'})
  end
end

task:add_args(get_commandline())
if command == 'run' then
  local args = config['program-args']
  if type(args) ~= 'nil' then
    task:add_args({'--'})
    task:add_args(args)
  end
end
register_task(task)
