project('metroskrew', 'c',
  default_options: ['warning_level=2'])

c = meson.get_compiler('c')

is_windows = c.has_define('_WIN32')

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
  '-U_FILE_OFFSET_BITS'
]

as_args_base = [
  arch,
  is_windows ? ['--defsym', '_WIN32=1'] : []
]

subdir('dlls')
subdir('bins')
subdir('patch')
subdir('relink')
subdir('test')
