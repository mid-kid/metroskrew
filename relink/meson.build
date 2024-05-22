fs = import('fs')

assert(host_machine.cpu_family() == 'x86',
  'This project can only be built for 32-bit x86')

sources = [
  'entry.s',
  'utils.s',
  'sysdeps.c',
  'dlls/advapi32.s',
  'dlls/kernel32.s',
  'dlls/lmgr8c.s',
  'dlls/user32.s',
  'dlls/version.s',
  'dlls/ws2_32.s',
  'dlls/ole32.s'
]

arch = '-march=i386'

common = static_library('common',
  sources,
  c_args: arch,
  pic: false)

foreach tool, bin : tools
  prog = custom_target(
    command: [exedump, '@INPUT@'],
    input: bin,
    output: tool + '.s',
    capture: true,
    build_by_default: true)

  ldscript = tool + '.ld'

  executable(tool,
    prog, tool + '_patch.s',
    name_suffix: 'exe',
    c_args: arch,
    link_args: [
      arch,
      '-Wl,-z,noexecstack',
      '-no-pie',
      '-T', meson.current_source_dir() / ldscript
    ],
    link_whole: common,
    link_depends: ldscript,
    include_directories: inc_root,
    install: true)
endforeach
