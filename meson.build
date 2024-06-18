project('metroskrew', 'c',
  default_options: ['warning_level=2'])

exedump = executable('main', 'main.c', native: true)
exescan = executable('scan', 'scan.c', native: true)

tools = {
  'mwccarm': files('bins/mwccarm.exe'),
  'mwasmarm': files('bins/mwasmarm.exe'),
  'mwldarm': files('bins/mwldarm.exe')
}

custom_target(
  output: 'scan.i',
  input: tools['mwccarm'],
  command: [exescan, '@OUTPUT@', '@INPUT@'])

subdir('relink')
subdir('test')
