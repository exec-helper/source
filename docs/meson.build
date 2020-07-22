docs_aliases = []

if get_option('usage-documentation')
  subdir('usage')
endif

if get_option('api-documentation')
  subdir('api')
endif

if get_option('usage-documentation') or get_option('api-documentation')
  alias_target('docs', docs_aliases)
endif
