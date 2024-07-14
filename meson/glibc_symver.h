#ifdef __i386__
// older version will try to execute files without shebangs as scripts,
// which is fine.
__asm__(".symver posix_spawn, posix_spawn@GLIBC_2.2");  // GLIBC_2.15

// older glibc had a single symbol to dispatch multiple types of stat
__asm__(".symver __xstat, __xstat@GLIBC_2.0");
__asm__(".symver __fxstat, __fxstat@GLIBC_2.0");

struct stat;
extern int __xstat(int, const char *, struct stat *);
static inline int stat(const char *path, struct stat *statbuf)
{ return __xstat(3, path, statbuf); }
extern int __fxstat(int, int, struct stat *);
static inline int fstat(int fd, struct stat *statbuf)
{ return __fxstat(3, fd, statbuf); }
#endif
