project('metroskrew', 'c',
  default_options: ['warning_level=2'])

exedump = executable('dump', 'dump.c', native: true, build_by_default: false)

tools = {
  'mwccarm': files('bins/mwccarm.exe'),
  'mwasmarm': files('bins/mwasmarm.exe'),
  'mwldarm': files('bins/mwldarm.exe')
}

as = find_program('as')
as_gen = generator(as,
  arguments: [
    '@EXTRA_ARGS@',
    '--MD', '@OUTPUT@.d',
    '-o', '@OUTPUT@',
    '@INPUT@'
  ],
  output: '@PLAINNAME@.o',
  depfile: '@PLAINNAME@.o.d')

subdir('patch')
subdir('relink')
subdir('test')
