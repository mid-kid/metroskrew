headers_args = [
  '-I' + meson.current_source_dir(),
  '-I' + meson.current_build_dir()
]

headers_dep_lean = declare_dependency(
  compile_args: headers_args)

if has_widl
  # Bare minimum for windows.h
  headers_widl_base = widl_gen.process(
    'msxml.idl',
    'oaidl.idl',
    'objidl.idl',
    'objidlbase.idl',
    'oleidl.idl',
    'propidl.idl',
    'servprov.idl',
    'unknwn.idl',
    'urlmon.idl',
    'wtypes.idl')

  headers_dep = declare_dependency(
    dependencies: headers_dep_lean,
    sources: headers_widl_base)
endif
