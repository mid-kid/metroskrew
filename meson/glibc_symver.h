#ifdef __i386__
// Current target: GLIBC_2.2
// Tested with: GLIBC_2.36

// older version will try to execute files without shebangs as scripts,
// which is fine.
__asm__(".symver posix_spawn, posix_spawn@GLIBC_2.2");  // Upgraded in GLIBC_2.15

// older glibc had a single symbol to dispatch multiple types of stat
#ifndef __SANITIZE_ADDRESS__
#include <features.h>
#if __GLIBC__ > 2 || __GLIBC_MINOR__ >= 33
struct stat;
extern int __xstat(int, const char *, struct stat *);
extern int __fxstat(int, int, struct stat *);
extern int __xstat64(int, const char *, struct stat *);
extern int __fxstat64(int, int, struct stat *);
#if _FILE_OFFSET_BITS == 64
static inline int stat(const char *path, struct stat *statbuf)
{ return __xstat64(3, path, statbuf); }
static inline int fstat(int fd, struct stat *statbuf)
{ return __fxstat64(3, fd, statbuf); }
#else
static inline int stat(const char *path, struct stat *statbuf)
{ return __xstat(3, path, statbuf); }
static inline int fstat(int fd, struct stat *statbuf)
{ return __fxstat(3, fd, statbuf); }
#endif
#endif
#endif

#endif
