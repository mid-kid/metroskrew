bison_gen = generator(bison,
  output: ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
  arguments: ['-o', '@OUTPUT0@', '--defines=@OUTPUT1@', '@INPUT@'])
flex_gen = generator(flex,
  output: '@BASENAME@.yy.c',
  arguments: ['-o', '@OUTPUT@', '@INPUT@'])

config = configure_file(
  output: 'config.h',
  configuration: {
    '__WINE_CONFIG_H': true,
    'PACKAGE_VERSION': '"' + meson.project_version() + '"',
    'INCLUDEDIR': '"/usr/include"',
    'DLLDIR': '"/usr/lib/wine"',
    'BIN_TO_INCLUDEDIR': '"../include"',
    'BIN_TO_DLLDIR': '"../lib/wine"'
  })

widl = executable('widl', 
  'attribute.c',
  'client.c',
  'expr.c',
  'hash.c',
  'header.c',
  'proxy.c',
  'register.c',
  'server.c',
  'typegen.c',
  'typelib.c',
  'typetree.c',
  'utils.c',
  'widl.c',
  'write_msft.c',
  '../wrc/wpp.c',
  bison_gen.process('parser.y', '../wrc/ppy.y'),
  flex_gen.process('parser.l', '../wrc/ppl.l'),
  config,
  include_directories: [
    '../../include',
    '../wrc'
  ],
  build_by_default: false,
  native: true)

widl_gen = generator(widl,
  output: '@BASENAME@.h',
  arguments: [
    '-o', '@OUTPUT@',
    '-m32',
    '--nostdinc',
    '-L../../dlls/*',
    '-I../../include',
    '-D__WINESRC__',
    '@INPUT@'
  ])
