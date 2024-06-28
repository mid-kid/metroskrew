fs = import('fs')

exescan = executable('scan', 'scan.c', native: true, build_by_default: false)

bins_patch_gen = {}
foreach name, bin : bins
  patch = custom_target(
    command: [exescan,
      '--incbin', 'bins' / name + '.exe',
      '-o', '@OUTPUT@', '@INPUT@'],
    output: name + '_patch.s',
    input: bins[name])
  bins_patch_gen += {name: patch}
endforeach

patch_dir = meson.current_source_dir() / 'gen'

bins_patch = {}
foreach name, bin : bins
  patch_file = patch_dir / name + '_patch.s'
  if not fs.exists(patch_file)
    run_command(['install', '-d', fs.parent(patch_file)], check: true)
    run_command(['touch', patch_file], check: true)
  endif
  patch = files(patch_file)
  bins_patch += {name: patch}
endforeach

run_target('patchgen',
  command: ['install',
    '-Dt', meson.current_source_dir() / patch_dir,
    bins_patch_gen['mwccarm'],
    bins_patch_gen['mwasmarm'],
    bins_patch_gen['mwldarm']
  ])

patch_inc = meson.current_source_dir()
