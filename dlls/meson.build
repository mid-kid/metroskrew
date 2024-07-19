sources_s = [
  'advapi32.s',
  'kernel32.s',
  'ole32.s',
  'version.s',
  'ws2_32.s'
]
sources_c = [
  '_utils.c',
  'advapi32.c',
  'kernel32.c',
  'lmgr8c.c',
  'ole32.c',
  'user32.c',
  'version.c'
]

c_args = [
  c_args_base,
  get_option('debug') ? [] : '-DNDEBUG',
  get_option('trace') ? '-DETRACE' : []
]

as_args = [
  as_args_base,
  '-I' + meson.current_source_dir(),
  get_option('debug') ? [] : ['--defsym', 'NDEBUG=1'],
  get_option('trace') ? ['--defsym', 'ETRACE=1'] : [],
]

common_inc = meson.current_source_dir()

common = static_library('common',
  sources_c,
  as_gen.process(sources_s, extra_args: as_args),
  c_args: c_args,
  dependencies: wine_headers,
  build_by_default: false)
