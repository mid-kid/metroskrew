c_args = [
  c_args_base
]

link_args = [
  arch
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

executable('skrewrap', 'wrap.c',
  c_args: c_args,
  link_args: link_args,
  install: true)
