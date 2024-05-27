fs = import('fs')

assert(host_machine.cpu_family() == 'x86',
  'This project can only be built for 32-bit x86')
arch = '-march=i386'

sources_s = [
  'entry.s',
  'utils.s',
  'dlls/advapi32.s',
  'dlls/kernel32.s',
  'dlls/lmgr8c.s',
  'dlls/user32.s',
  'dlls/version.s',
  'dlls/ws2_32.s',
  'dlls/ole32.s'
]
sources_c = [
  'sysdeps.c'
]

as = find_program('as')

c_args = [
  arch,
  '-U_FILE_OFFSET_BITS',
  get_option('debug') ? [] : ['-DNDEBUG'],
  get_option('trace') ? ['-DTRACE'] : []
]

as_args = [
  arch,
  '-I', meson.current_source_dir(),
  get_option('debug') ? [] : ['--defsym', 'NDEBUG=1'],
  get_option('trace') ? ['--defsym', 'TRACE=1'] : []
]

bld_as = generator(as,
  arguments: [
    as_args,
    '@EXTRA_ARGS@',
    '--MD', '@OUTPUT@.d',
    '-o', '@OUTPUT@',
    '@INPUT@'
  ],
  output: '@PLAINNAME@.o',
  depfile: '@PLAINNAME@.o.d')

common = static_library('common',
  sources_c,
  objects: bld_as.process(sources_s),
  c_args: c_args)

foreach tool, bin : tools
  prog_src = custom_target(
    command: [exedump, '@INPUT@'],
    input: bin,
    output: tool + '.s',
    capture: true,
    build_by_default: true)
  prog = bld_as.process(
    prog_src, tool + '_patch.s',
    extra_args: ['-I', meson.project_source_root()])

  ldscript = tool + '.ld'

  executable(tool,
    objects: prog,
    name_suffix: 'exe',
    link_args: [
      arch,
      '-Wl,-z,noexecstack',
      '-no-pie',
      '-T', meson.current_source_dir() / ldscript
    ],
    link_whole: common,
    link_depends: ldscript,
    install: true)
endforeach
