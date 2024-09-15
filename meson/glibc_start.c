#ifdef __i386__
#include <features.h>
#if __GLIBC__ > 2 || __GLIBC_MINOR__ >= 34
// Link against the old __libc_start_main
__asm__(".symver old__libc_start_main, __libc_start_main@GLIBC_2.0");
__asm__(".symver new__libc_start_main, __libc_start_main@@_NEW");
__asm__(".global new__libc_start_main");
__asm__("new__libc_start_main: jmp old__libc_start_main");
#endif
#endif
