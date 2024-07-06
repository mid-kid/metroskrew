#include "_utils.h"

#include <string.h>

bool path_has_drv(const char *path)
{
    return path[0] >= 'A' && path[0] <= 'Z' &&
            path[1] == ':' && path[2] == '\\';
}

char *path_dup_unx(const char *path)
{
    const char *o = path;
#ifndef _WIN32
    if (path_has_drv(o)) o += 2;
#endif

    char *n = strcpy(malloc(strlen(o) + 1), o);
    for (char *c = n; *c; c++) if (*c == '\\') *c = '/';
    return n;
}
