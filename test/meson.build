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
tests_ld = [
  'basic_ld'
]

# test() requires a cross-compiled target in its argument list
#  in order to set MESON_EXE_WRAPPER.
dummy = static_library('dummy', objects: 'test.sh', build_by_default: false)

test_ld_o = custom_target(
  output: 'test_ld.o',
  input: 'basic_c.c',
  command: ['sh', '-c', '"$@" >/dev/null', '--',
    bins_relink['mwccarm-4.0-1051'],
    '-c', '-o', '@OUTPUT@', '@INPUT@'])

foreach name, bin : bins
  if name.startswith('mwccarm-')
    tests = tests_cc
    tests_dep = []
    tests_env = {
      'MWCCARM_VER': name,
      'MWCCARM': bins_relink[name].full_path()
    }
  elif name.startswith('mwldarm-')
    tests = tests_ld
    tests_dep = test_ld_o
    tests_env = {
      'MWLDARM_VER': name,
      'MWLDARM': bins_relink[name].full_path()
    }
  elif name.startswith('mwasmarm-')
    tests = tests_as
    tests_dep = []
    tests_env = {
      'MWASMARM_VER': name,
      'MWASMARM': bins_relink[name].full_path()
    }
  else
    continue
  endif

  foreach test_name : tests
    test(name + '_' + test_name, files('test.sh'),
      args: [test_name, dummy],
      workdir: meson.current_build_dir(),
      depends: [
        tests_dep,
        bins_relink[name]
      ],
      env: tests_env + {
        'SRC': meson.current_source_dir()
      })
  endforeach
endforeach
