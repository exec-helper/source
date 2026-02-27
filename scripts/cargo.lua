local command = one(config['command'])
if type(command) == 'nil' then
  input_error('You must define a command when using the cargo plugin!')
end

task:add_args({'cargo', command})

if command == 'build' or command == 'run' then
  local profile = one(config['profile'])
  if profile then
    task:add_args({'--profile', profile})
  end

  local target = one(config['target'])
  if target then
    task:add_args({'--target', target})
  end
end

if command == 'build' or command == 'run' or command == 'test' or command == 'clippy' then
  if one(config['all-targets']) then
    task:add_args({'--all-targets'})
  end

  local color = one(config['color'])
  if color then
    task:add_args({'--color', color})
  end
end

task:add_args(get_commandline())

task:add_args({'--'})

if command == 'clippy' and one(config['fail-on-warnings']) then
  task:add_args({'--deny', 'warnings'})
end

if command == 'run' then
  local args = config['program-args']
  if args then
    task:add_args(args)
  end
end
task:register()
