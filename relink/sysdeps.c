#include <assert.h>
#include <stddef.h>
#include <sys/stat.h>

size_t sys_stat_sizeof = sizeof(struct stat);

size_t sys_stat_mode_offsetof = offsetof(struct stat, st_mode);
static_assert(sizeof(mode_t) == 4, "");
size_t sys_stat_size_offsetof = offsetof(struct stat, st_size);
static_assert(sizeof(off_t) == 4, "");
