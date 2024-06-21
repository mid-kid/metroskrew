#include <string.h>
#include <stdlib.h>

char *path_dup_unx(const char *path)
{
    const char *o = path;
    if (o[0] == 'Z' && o[1] == ':' && o[2] == '\\') o += 2;

    char *n = strcpy(malloc(strlen(o) + 1), o);
    for (char *c = n; *c; c++) if (*c == '\\') *c = '/';
    return n;
}
