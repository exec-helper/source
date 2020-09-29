src = [
  'src/catch.cpp',
]

deps = [
  log,
  catch2_dep,
]

catch2_lib = library('exec-helper-catch-main', src,
  include_directories : ['include', 'include/unittest'],
  dependencies : deps,
)

unittest = declare_dependency(
  include_directories: ['include'],
  link_with: catch2_lib,
  dependencies: deps
)

subdir('include/unittest')
