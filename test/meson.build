tests = [
  'basic_c',
  'basic_s',
  'include',
  'include_dir',
  'include_sys',
  'switch_float_bug'
]

# test() requires a cross-compiled target in its argument list
#  in order to set MESON_EXE_WRAPPER.
dummy = static_library('dummy', objects: 'test.sh', build_by_default: false)

#foreach version, vername : versions
#  foreach name : tests
#    test(name, files('test.sh'),
#      args: [name, dummy],
#      workdir: meson.current_build_dir(),
#      depends: [
#        bins_relink['mwccarm-' + vername],
#        bins_relink['mwasmarm-' + vername],
#        bins_relink['mwldarm-' + vername]
#      ],
#      env: {
#        'SRC': meson.current_source_dir(),
#        'VER': '-' + vername,
#        'MWCCARM': bins_relink['mwccarm-' + vername].full_path(),
#        'MWASMARM': bins_relink['mwasmarm-' + vername].full_path(),
#        'MWLDARM': bins_relink['mwldarm-' + vername].full_path()
#      },
#    )
#  endforeach
#endforeach
