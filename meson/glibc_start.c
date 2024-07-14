#ifdef __i386__
// Link against the old __libc_start_main
__asm__(".symver old__libc_start_main, __libc_start_main@GLIBC_2.0");
__asm__(".symver new__libc_start_main, __libc_start_main@@_NEW");
__asm__(".global new__libc_start_main");
__asm__("new__libc_start_main: jmp old__libc_start_main@PLT");
#endif
