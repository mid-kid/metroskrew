fs = import('fs')

exescan = executable('scan', 'scan.c', native: true, build_by_default: false)

bins_patch_gen = {}
foreach name, bin : bins
  patch = custom_target(
    command: [exescan,
      '--incbin', bins_paths[name],
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

bins_patch_gen_files = []
foreach name, patch : bins_patch_gen
  bins_patch_gen_files += patch
endforeach
run_target('patchgen',
  command: ['install',
    '-Dm644', '-t', meson.current_source_dir() / patch_dir,
    bins_patch_gen_files
  ])

patch_inc = meson.current_source_dir()
