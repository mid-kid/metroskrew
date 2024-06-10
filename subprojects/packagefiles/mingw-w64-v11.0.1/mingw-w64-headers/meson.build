subdir('crt')

headers_inc_dir = meson.current_source_dir() / 'include'
headers_defaults_dir = meson.current_source_dir() / 'defaults' / 'include'
headers_crt_bdir = meson.current_build_dir() / 'crt'

headers_args = [
  '-I' + headers_inc_dir,
  '-I' + headers_defaults_dir,
  '-I' + headers_crt_bdir,
  '-D__CYGWIN__',
  '-D_X86_',
  '-D__cdecl=__attribute__((__cdecl__)) __attribute__((__force_align_arg_pointer__))',
  '-D__stdcall=__attribute__((__stdcall__)) __attribute__((__force_align_arg_pointer__))',
  '-D__declspec(x)=__declspec_##x',
  '-D__declspec_dllimport=',
  '-D__declspec_noreturn=__attribute__((noreturn))',
  '-Wno-expansion-to-defined'
]

headers_dep = declare_dependency(
  compile_args: headers_args)
