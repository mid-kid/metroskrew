exedump = executable('dump', 'dump.c', native: true, build_by_default: false)

sources_s = [
  'entry.s'
]

sources_c = [
]

c_args = [
  c_args_base,
  get_option('debug') ? [] : '-DNDEBUG',
  get_option('trace') ? '-DETRACE' : [],
  '-I' + common_inc
]

as_args = [
  as_args_base,
  '-I' + common_inc
]

link_args = [
  link_args_base,
  '-Wl,-Ttext-segment,0x00400000',
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

relink_common = static_library('relink_common', sources_c,
  as_gen.process(sources_s, extra_args: as_args),
  c_args: c_args)

ldscript_cfg = configuration_data()
ldscript_cfg.set('INSERT', '.init')
if is_windows
  ldscript_cfg.set('INSERT', '.text')
endif

ldscript_dep = configure_file(
  input: 'link.ld.in',
  output: '@BASENAME@',
  configuration: ldscript_cfg)
ldscript = meson.current_build_dir() / 'link.ld'
dllimport = meson.current_source_dir() / 'dllimport.ld'

bins_relink = {}
foreach name, bin : bins
  prog_src = custom_target(
    command: [exedump,
      is_windows ? '--win' : [],
      '-o', '@OUTPUT@', '@INPUT@'
    ],
    input: bin,
    output: name + '.s')
  prog_obj = as_gen.process(prog_src, bins_patch[name],
    extra_args: [
      as_args,
      '-I' + patch_inc,
      '-I' + bins_dir
    ])

  prog = executable(name,
    prog_obj,
    name_suffix: 'exe',
    link_args: [
      link_args,
      '-T' + ldscript,
      is_windows ?
        '-T' + dllimport : [],
      name.startswith('mwasmarm-') ?
        '-Wl,-Ttext-segment=0x00b80000' : [],
      is_windows and name.startswith('mwasmarm-') ?
        '-Wl,--image-base,0x00b80000' : [],
    ],
    link_with: [patch_common],
    link_whole: [relink_common, common],
    link_depends: [
      ldscript_dep,
      is_windows ? dllimport : []
    ],
    install: true)
  bins_relink += {name: prog}
endforeach
