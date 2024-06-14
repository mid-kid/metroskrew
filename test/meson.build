tests = [
  'basic_c',
  'switch_float_bug'
]

# test() requires a cross-compiled target in its argument list
#  in order to set MESON_EXE_WRAPPER.
dummy = static_library('dummy', objects: 'test.sh')

foreach name : tests
  test(name, files('test.sh'),
    args: [name, dummy],
    workdir: meson.current_build_dir(),
    depends: [
      tools_relink['mwccarm'],
      tools_relink['mwasmarm']
    ],
    env: {
      'SRC': meson.current_source_dir(),
      'MWCCARM': tools_relink['mwccarm'].full_path(),
      'MWASMARM': tools_relink['mwasmarm'].full_path()
    },
  )
endforeach
