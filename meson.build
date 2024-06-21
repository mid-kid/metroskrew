project('metroskrew', 'c',
  default_options: ['warning_level=2'])

exedump = executable('dump', 'dump.c', native: true, build_by_default: false)
exescan = executable('scan', 'scan.c', native: true, build_by_default: false)

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
