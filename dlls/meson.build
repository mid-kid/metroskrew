assert(host_machine.cpu_family() == 'x86',
  'This project can only be built for 32-bit x86')
arch = '-march=i586'

sources_s = [
  'advapi32.s',
  'kernel32.s',
  'ole32.s',
  'user32.s',
  'version.s',
  'ws2_32.s'
]
sources_c = [
  '_utils.c',
  'advapi32.c',
  'kernel32.c',
  'lmgr8c.c',
  'user32.c',
  'version.c'
]

win_headers = subproject('wine').get_variable('headers_dep_lean')

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

common = static_library('common',
  sources_c,
  objects: as_gen.process(sources_s,
    extra_args: as_args),
  c_args: c_args,
  dependencies: win_headers,
  build_by_default: false)

common_inc = meson.current_source_dir()
