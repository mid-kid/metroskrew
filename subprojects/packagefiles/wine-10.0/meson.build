project('wine', 'c',
  version: '10.0',
  default_options: ['warning_level=1'])

bison = find_program('bison', native: true,
  required: get_option('with_widl'))
flex = find_program('flex', native: true,
  required: get_option('with_widl'))

has_widl = bison.found() and flex.found()

if has_widl
  subdir('tools/widl')
endif

subdir('include')

if has_widl
  static_library('wine_test',
    'test.c',
    dependencies: headers_dep,
    build_by_default: false)
endif
