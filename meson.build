project('metroskrew', 'c',
  default_options: ['warning_level=3'])

exedump = executable('main', 'main.c', native: true)

tools = {
  'mwccarm': files('bins/mwccarm.exe'),
  'mwasmarm': files('bins/mwasmarm.exe')
}

subdir('relink')