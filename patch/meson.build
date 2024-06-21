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

patch_dir = 'gen'

tools_patch = {}
foreach tool, bin : tools
  if fs.exists(patch_dir / tool + '_patch.s')
    patch = files(patch_dir / tool + '_patch.s')
  else
    patch = tools_patch_gen[tool]
  endif
  tools_patch += {tool: patch}
endforeach

patch_inc = meson.current_source_dir()
