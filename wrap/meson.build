prog_name = 'skrewrap'

default_mwccarm = '4.0-1051'
default_mwldarm = '2.0-99'
default_mwasmarm = '1.0-27'

cfg = configuration_data()
cfg.set_quoted('PROGRAM_NAME', prog_name)
cfg.set_quoted('BIN_TO_LIBDIR', proj_bin_to_libdir)
cfg.set_quoted('BIN_TO_DATADIR', proj_bin_to_datadir)
cfg.set_quoted('DEFAULT_MWCCARM', default_mwccarm)
cfg.set_quoted('DEFAULT_MWLDARM', default_mwldarm)
cfg.set_quoted('DEFAULT_MWASMARM', default_mwasmarm)

cfg_file = configure_file(output: 'config.h', configuration: cfg)

c_args = [
  c_args_base
]

link_args = [
  link_args_base,
  '-no-pie'
]

if is_windows
  c_args += [
    '-municode',
    '-D_UNICODE',
    '-D_CRT_SECURE_NO_WARNINGS'
  ]
  link_args += '-municode'
endif

wrap_progs = ['skrewrap', 'mwccarm', 'mwldarm', 'mwasmarm']
wrap = []
foreach wrap_prog : wrap_progs
  wrap += executable(wrap_prog, 'wrap.c', cfg_file,
    c_args: c_args + [
      wrap_prog != 'skrewrap' ? '-DWRAP_PROG="' + wrap_prog + '"' : []
    ],
    link_args: link_args,
    install: true)
endforeach

# test() requires a cross-compiled target in its argument list
#  in order to set MESON_EXE_WRAPPER.
dummy = static_library('dummy', objects: 'test.sh', build_by_default: false)

tests = [
  'exec_argv',
  'exec_path',
  'sdk'
]

test_mwccarm = bins_relink['mwccarm-' + default_mwccarm]
test_mwldarm = bins_relink['mwldarm-' + default_mwldarm]
test_mwasmarm = bins_relink['mwasmarm-' + default_mwasmarm]
foreach name : tests
  test('wrap_' + name, files('test.sh'),
    args: [name, dummy],
    workdir: meson.current_build_dir(),
    depends: [
      wrap[0],
      test_mwccarm,
      test_mwldarm,
      test_mwasmarm
    ],
    env: {
      'PROGRAM': wrap[0].full_path(),
      'MWCCARM': test_mwccarm.full_path(),
      'MWLDARM': test_mwldarm.full_path(),
      'MWASMARM': test_mwasmarm.full_path()
    })
endforeach
