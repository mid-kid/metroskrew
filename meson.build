project('metroskrew', 'c',
  default_options: ['warning_level=2'])

wine_headers = subproject('wine').get_variable('headers_dep_lean')
bins_sub = subproject('mw-executables')
bins_dir = bins_sub.get_variable('bins_dir')
bins = bins_sub.get_variable('bins')
bins_paths = bins_sub.get_variable('bins_paths')
bins_paths_all = bins_sub.get_variable('bins_paths_all')

c = meson.get_compiler('c')

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

assert(host_machine.cpu_family() == 'x86',
  'This project can only be built for 32-bit x86')
arch = '-march=i586'

c_args_base = [
  arch,
  '-fno-PIC',
  '-fno-plt',
  '-U_FILE_OFFSET_BITS',
  '-U_FORTIFY_SOURCE'
]

as_args_base = [
  arch
]

link_args_base = [
  arch,
  '-no-pie'
]

is_glibc = c.has_define('__GLIBC__', prefix: '#include <features.h>')
is_windows = c.has_define('_WIN32')

if is_glibc
  c_args_base += ['-include',
    meson.current_source_dir() / 'meson' / 'glibc_symver.h']
  link_args_base += meson.current_source_dir() / 'meson' / 'glibc_start.c'
endif

if is_windows
  as_args_base += ['--defsym', '_WIN32=1']
  c_args_base += ['-mcrtdll=msvcrt-os', '-D_WIN32_WINNT=0x500']
  link_args_base += '-mcrtdll=msvcrt-os'
endif

proj_datadir = get_option('datadir') / meson.project_name()

subdir('dlls')
subdir('patch')
subdir('relink')
subdir('test')
subdir('data')
subdir('wrap')
