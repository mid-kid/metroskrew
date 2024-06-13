tests = [
  'switch_float_bug'
]

sh = find_program('bash', native:false)

foreach name : tests
  test(name, sh,
    args: [meson.current_source_dir() / 'test.sh', name],
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
