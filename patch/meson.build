fs = import('fs')

exescan = executable('scan', 'scan.c', native: true, build_by_default: false)

tools_patch_gen = {}
foreach tool, bin : tools
  patch = custom_target(
    command: [exescan,
      '--incbin', 'bins' / tool + '.exe',
      '-o', '@OUTPUT@', '@INPUT@'],
    output: tool + '_patch.s',
    input: tools[tool])
  tools_patch_gen += {tool: patch}
endforeach

patch_dir = meson.current_source_dir() / 'gen'

tools_patch = {}
foreach tool, bin : tools
  patch_file = patch_dir / tool + '_patch.s'
  if not fs.exists(patch_file)
    run_command(['install', '-d', fs.parent(patch_file)], check: true)
    run_command(['touch', patch_file], check: true)
  endif
  patch = files(patch_file)
  tools_patch += {tool: patch}
endforeach

run_target('patchgen',
  command: ['install',
    '-Dt', meson.current_source_dir() / patch_dir,
    tools_patch_gen['mwccarm'],
    tools_patch_gen['mwasmarm'],
    tools_patch_gen['mwldarm']
  ])

patch_inc = meson.current_source_dir()
