fs = import('fs')

assert(host_machine.cpu_family() == 'x86',
  'This project can only be built for 32-bit x86')
arch = '-march=i586'

sources_s = [
  'entry.s',
  'dlls/advapi32.s',
  'dlls/kernel32.s',
  'dlls/user32.s',
  'dlls/version.s',
  'dlls/ws2_32.s',
  'dlls/ole32.s'
]
sources_c = [
  'utils.c',
  'dlls/kernel32.c',
  'dlls/user32.c',
  'dlls/lmgr8c.c',
  'dlls/version.c'
]

c = meson.get_compiler('c')
as = find_program('as')

win_headers = subproject('wine').get_variable('headers_dep_lean')

is_windows = c.has_define('_WIN32')

c_args = [
  arch,
  '-fno-PIC',
  '-U_FILE_OFFSET_BITS',
  get_option('debug') ? [] : '-DNDEBUG',
  get_option('trace') ? '-DETRACE' : []
]

as_args = [
  arch,
  '-I' + meson.current_source_dir(),
  get_option('debug') ? [] : ['--defsym', 'NDEBUG=1'],
  get_option('trace') ? ['--defsym', 'ETRACE=1'] : [],
  is_windows ? ['--defsym', '_WIN32=1'] : []
]

link_args = [
  arch,
  '-no-pie',
  c.get_supported_link_arguments('-Wl,-z,noexecstack')
]

if is_windows
  sources_s += 'dllimport.s'
  link_args += ['-lversion', '-lws2_32']
  link_args += [
    '-Wl,--enable-stdcall-fixup',
    '-Wl,--image-base,0x00400000',
    '-Wl,--section-alignment,0x1000'
  ]
endif

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
  c_args: c_args,
  dependencies: win_headers)

tools_relink = {}
foreach tool, bin : tools
  prog_src = custom_target(
    command: [exedump, is_windows ? '--win' : [], '@INPUT@'],
    input: bin,
    output: tool + '.s',
    capture: true,
    build_by_default: true)
  prog_obj = bld_as.process(
    prog_src, tool + '_patch.s',
    extra_args: '-I' + meson.project_source_root())

  ldscript = meson.current_source_dir() / tool + '.ld'
  dllimport = meson.current_source_dir() / 'dllimport.ld'

  prog = executable(tool,
    objects: prog_obj,
    name_suffix: 'exe',
    link_args: [
      '-T' + ldscript,
      is_windows ? '-T' + dllimport : [],
      link_args
    ],
    link_whole: common,
    link_depends: [
      ldscript,
      is_windows ? dllimport : []
    ],
    install: true)
  tools_relink += {tool: prog}
endforeach
