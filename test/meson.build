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

foreach name : tests
  test(name, files('test.sh'),
    args: [name, dummy],
    workdir: meson.current_build_dir(),
    depends: [
      tools_relink['mwccarm'],
      tools_relink['mwasmarm'],
      tools_relink['mwldarm']
    ],
    env: {
      'SRC': meson.current_source_dir(),
      'MWCCARM': tools_relink['mwccarm'].full_path(),
      'MWASMARM': tools_relink['mwasmarm'].full_path(),
      'MWLDARM': tools_relink['mwldarm'].full_path()
    },
  )
endforeach
