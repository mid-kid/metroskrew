project('metroskrew', 'c',
  default_options: ['warning_level=2'],
  meson_version: '>=0.60.0')

wine_headers = subproject('wine').get_variable('headers_dep_lean')
bins_sub = subproject('mw-executables')
bins_dir = bins_sub.get_variable('bins_dir')
bins = bins_sub.get_variable('bins')
bins_paths = bins_sub.get_variable('bins_paths')
bins_paths_all = bins_sub.get_variable('bins_paths_all')

c = meson.get_compiler('c')

python = find_program('python3')

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

support_dir = meson.current_source_dir() / 'meson'

is_glibc = c.get_define('__GLIBC__', prefix: '#include <stdlib.h>') != ''
is_windows = c.get_define('_WIN32') != ''

if is_glibc
  tested = c.compiles('', args: [
    '-include', support_dir / 'glibc_symver.h',
    '-include', support_dir / 'glibc_start.c'])
  if tested
    c_args_base = ['-include', support_dir / 'glibc_symver.h']
    link_args_base = support_dir / 'glibc_start.c'
    message('glibc downgrade: YES')
  else
    message('glibc downgrade: NO')
  endif
endif

if is_windows
  as_args_base += ['--defsym', '_WIN32=1']
  c_args_base += ['-mcrtdll=msvcrt-os', '-D_WIN32_WINNT=0x500']
  link_args_base += '-mcrtdll=msvcrt-os'
endif

proj_libdir = get_option('libdir') / meson.project_name()
proj_datadir = get_option('datadir') / meson.project_name()
proj_bin_to_libdir = run_command(
  [python, 'meson' / 'relpath.py', proj_libdir, get_option('bindir')],
  check: true).stdout().strip()
proj_bin_to_datadir = run_command(
  [python, 'meson' / 'relpath.py', proj_datadir, get_option('bindir')],
  check: true).stdout().strip()

subdir('dlls')
subdir('patch')
subdir('relink')
subdir('test')
subdir('data')
subdir('wrap')
