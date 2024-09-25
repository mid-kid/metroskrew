fs = import('fs')

# Common code used by the patches

sources_c = [
  'env.c',
  'memreuse01.c'
]

c_args = [
  c_args_base,
  get_option('debug') ? [] : '-DNDEBUG',
  get_option('trace') ? '-DETRACE' : [],
  '-I' + common_inc
]

patch_common = static_library('patch_common', sources_c,
  c_args: c_args)

# Patch listing

patch_dir = meson.current_source_dir() / 'gen'
patch_inc = meson.current_source_dir()

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

# Patch generation

exescan = executable('scan', 'scan.c', native: true, build_by_default: false)

bins_patch_gen = {}
bins_patch_gen_files = []
foreach name, bin : bins
  patch = custom_target(
    command: [exescan,
      '--incbin', bins_paths[name],
      '-o', '@OUTPUT@', '@INPUT@'],
    output: name + '_patch.s',
    input: bins[name])
  bins_patch_gen += {name: patch}
  bins_patch_gen_files += patch
endforeach

run_target('patchgen',
  command: ['install',
    '-Dm644', '-t', meson.current_source_dir() / patch_dir,
    bins_patch_gen_files
  ])
