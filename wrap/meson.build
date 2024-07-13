fs = import('fs')

c_args = [
  c_args_base
]

link_args = [
  link_args_base
]

if host_machine.system() == 'windows'
  winflags = [
    '-mconsole',
    '-municode'
  ]
  link_args += winflags
  c_args += [
    winflags,
    '-DUNICODE',
    '-D_UNICODE',
    '-D_CRT_SECURE_NO_WARNINGS'
  ]
endif

wrap_name = 'skrewrap'

cfg = configuration_data()
cfg.set_quoted('PROGRAM_NAME', wrap_name)
cfg.set_quoted('BIN_TO_DATADIR',
  fs.relative_to(proj_datadir, get_option('bindir')))

cfg_file = configure_file(output: 'config.h', configuration: cfg)

executable(wrap_name, 'wrap.c', cfg_file,
  c_args: c_args,
  link_args: link_args,
  install: true)
