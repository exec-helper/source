fs = import('fs')

sphinx = find_program('sphinx-build')

man1_sources = [
  '../../src/applications/exec-helper.rst'
]

man5_sources = [
  '../../src/config/docs/exec-helper-config.rst',
  '../../src/config/docs/exec-helper-config-patterns.rst',
  '../../src/config/docs/exec-helper-config-environment.rst',
  '../../src/plugins/docs/exec-helper-custom-plugins.rst',
  '../../src/plugins/docs/exec-helper-plugins-pmd.rst',
  '../../src/plugins/docs/exec-helper-plugins-valgrind.rst',
  '../../src/plugins/docs/exec-helper-plugins-execute-plugin.rst',
  '../../src/plugins/docs/exec-helper-plugins-selector.rst',
  '../../src/plugins/docs/exec-helper-plugins-cppcheck.rst',
  '../../src/plugins/docs/exec-helper-plugins.rst',
  '../../src/plugins/docs/exec-helper-plugins-bootstrap.rst',
  '../../src/plugins/docs/exec-helper-plugins-scons.rst',
  '../../src/plugins/docs/exec-helper-plugins-make.rst',
  '../../src/plugins/docs/exec-helper-plugins-cmake.rst',
  '../../src/plugins/docs/exec-helper-plugins-docker.rst',
  '../../src/plugins/docs/exec-helper-plugins-clang-tidy.rst',
  '../../src/plugins/docs/exec-helper-plugins-command-line-command.rst',
  '../../src/plugins/docs/exec-helper-plugins-clang-static-analyzer.rst',
  '../../src/plugins/docs/exec-helper-plugins-lcov.rst',
  '../../src/plugins/docs/exec-helper-plugins-sh.rst',
  '../../src/plugins/docs/exec-helper-plugins-bash.rst',
  '../../src/plugins/docs/exec-helper-plugins-zsh.rst',
  '../../src/plugins/docs/exec-helper-plugins-fish.rst',
]

sources = man1_sources + man5_sources

usage_aliases = []

# ---------------------- Generate MAN documentation ---------------------------------------------------
if get_option('usage-documentation-man')
  man1_intermediate = ['eh.1']
  man1_output = ['eh.1.gz']
  man1_install = [join_paths(get_option('mandir'), 'man1')]

  foreach source : man1_sources
    man1_intermediate += fs.replace_suffix(fs.name(source), '.1')
    man1_output +=  fs.replace_suffix(fs.name(source), '.1.gz')
    man1_install += join_paths(get_option('mandir'), 'man1')
  endforeach

  man5_intermediate = []
  man5_output = []
  man5_install = []

  foreach source : man5_sources
    man5_intermediate += fs.replace_suffix(fs.name(source), '.5')
    man5_output += fs.replace_suffix(fs.name(source), '.5.gz')
    man5_install += join_paths(get_option('mandir'), 'man5')
  endforeach

  gzip = find_program('gzip')

  man = custom_target('Building man documentation',
    command: [sphinx, '-q', '-b', 'man', meson.source_root(), meson.current_build_dir(), '&&', 'cd', meson.current_build_dir(), '&&', gzip, '--keep', '--force', '--best'] + man1_intermediate + man5_intermediate,
    input: man1_sources + man5_sources,
    output: man1_output + man5_output,
    build_by_default: true,
    install: true,
    install_dir: man1_install + man5_install,
  )

  usage_aliases += man
endif

# ---------------------- Generate HTML documentation ---------------------------------------------------
if get_option('usage-documentation-html')
  html = custom_target('Building html documentation',
    command: [sphinx, '-q', '-b', 'html', meson.source_root(), meson.current_build_dir() + '/html'],
    input: ['conf.py'] + sources,
    output: 'html',
    build_by_default: true,
    install: true,
    install_dir : get_option('datadir') / 'doc' / 'exec-helper'
  )

  usage_aliases += html
endif

docs_aliases += usage_aliases
usage = alias_target('usage', usage_aliases)
