exedump = executable('dump', 'dump.c', native: true, build_by_default: false)

sources_s = [
  'entry.s'
]

c_args = [
  c_args_base
]

as_args = [
  as_args_base,
  '-I' + common_inc
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

relink_common = as_gen.process(sources_s,
  extra_args: as_args)

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

  ldscript = meson.current_source_dir() / 'link.ld'
  dllimport = meson.current_source_dir() / 'dllimport.ld'

  prog = executable(name,
    objects: [relink_common, prog_obj],
    name_suffix: 'exe',
    c_args: c_args,
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
  bins_relink += {name: prog}
endforeach
