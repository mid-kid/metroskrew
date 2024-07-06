if host_machine.system() == 'windows'
  winflags = [
    '-mconsole',
    '-municode'
  ]
  add_project_arguments(winflags, language: 'c')
  add_project_link_arguments(winflags, language: 'c')
  add_project_arguments(
    '-DUNICODE',
    '-D_UNICODE',
    '-D_CRT_SECURE_NO_WARNINGS',
    language: 'c')
endif

executable('skrewrap', 'wrap.c',
  install: true)
