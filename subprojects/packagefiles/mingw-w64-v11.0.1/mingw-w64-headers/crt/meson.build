# Values from configure.ac
mingw_cfg = configuration_data({
  'DEFAULT_MSVCRT_VERSION': '0x700',
  'DEFAULT_WIN32_WINNT': '0xa00'
})

configure_file(
  input: '_mingw.h.in',
  output: '_mingw.h',
  configuration: mingw_cfg
)

# Expose only the minimum set of headers
copy_headers = [
  '_cygwin.h',
  '_mingw_mac.h',
  '_mingw_secapi.h',
  '_mingw_unicode.h',
  'corecrt.h',
  'crtdefs.h',
  'excpt.h',
  'vadefs.h'
]

foreach x : copy_headers
  configure_file(
    input: x,
    output: x,
    copy: true
  )
endforeach
