if get_option('changelog')
  gitchangelog = find_program('gitchangelog')

  custom_target('changelog', 
    input: '.gitchangelog.rc',
    output: 'exec-helper.changelog',
    command: gitchangelog,
    capture: true,
    install: true,
    install_dir: get_option('datadir') / 'doc' / 'exec-helper',
    build_by_default: true,
  )
endif
