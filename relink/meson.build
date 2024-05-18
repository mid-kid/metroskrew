fs = import('fs')

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

common = static_library('common', sources, pic: false)

foreach tool, bin : tools
  prog_dump = custom_target(
    command: [exedump, '@INPUT@'],
    input: bin,
    output: tool + '.s',
    capture: true,
    build_by_default: true
  )
  prog = declare_dependency(
    link_whole: common,
    sources: [prog_dump, tool + '_patch.s']
  )

  # TODO: How to remove this hack?
  ldscript = fs.copyfile(tool + '.ld')

  executable(tool,
    dependencies: prog,
    name_suffix: 'exe',
    link_args: ['-Wl,-z,noexecstack', '-no-pie', '-T', ldscript.full_path()],
    link_depends: ldscript,
    include_directories: inc_root,
    install: true
  )
endforeach
