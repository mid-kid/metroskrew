project('metroskrew', 'c',
  default_options: ['warning_level=3'])
add_global_arguments('-U_FILE_OFFSET_BITS', language: 'c')

exedump = executable('main', 'main.c', native: true)

tools = {
  'mwccarm': files('bins/mwccarm.exe'),
  'mwasmarm': files('bins/mwasmarm.exe')
}

inc_root = include_directories('.')

subdir('relink')
