tests_cc = [
  'basic_c',
  'include',
  'include_dir',
  'include_sys',
  'switch_float_bug'
]
tests_as = [
  'basic_s'
]

# test() requires a cross-compiled target in its argument list
#  in order to set MESON_EXE_WRAPPER.
dummy = static_library('dummy', objects: 'test.sh', build_by_default: false)

foreach name, bin : bins
  if name.startswith('mwccarm-')
    tests = tests_cc
    tests_dep = []
    tests_env = {
      'VER': name.substring(7),
      'MWCCARM': bins_relink[name].full_path()
    }
  elif name.startswith('mwasmarm-')
    tests = tests_as
    tests_dep = []
    tests_env = {
      'VER': name.substring(8),
      'MWASMARM': bins_relink[name].full_path()
    }
  endif

  foreach test_name : tests
    test(name + '_' + test_name, files('test.sh'),
      args: [test_name, dummy],
      workdir: meson.current_build_dir(),
      depends: tests_dep + [
        bins_relink[name]
      ],
      env: tests_env + {
        'SRC': meson.current_source_dir()
      }
    )
  endforeach
endforeach
