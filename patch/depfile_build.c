#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include.h"

// This set of functions is responsible for emitting makefile-style dependency
// information, when using the -M/-MM/-MD/-MMD family of options.

// By default, metrowerks outputs a full path to each included header. This
// causes several problems. The full path isn't recognized to be equivalent to
// a relative rule generating a header, thus "make" will need to be called
// twice to fully rebuild everything. A full path also needs to be translated
// when crossing environment boundaries (e.g. Running windows metroskrew from
// WSL1, or some cygwin/msys environment). Additionally, backslashes are used
// as a path separator on linux, where they shouldn't be.

#define SKREW_FIX_DEPFILES

#ifdef SKREW_FIX_DEPFILES
#include <unistd.h>
#endif

__cdecl void *malloc_clear(size_t size);  // 0x00425ac0

// 0x004110f0
__stdcall char *path_join(mwpath *src, char *dst, int dst_size)
{
    // Allocate memory
    if (dst_size == 0) dst_size = PATH_MAX;
    if (dst == NULL) {
        dst = malloc_clear(dst_size);
        if (dst == NULL) return NULL;
    }

    // Truncate as necessary
    int len_dir = strlen(src->dir);
    int len_file = strlen(src->file);
    if (len_file + len_dir >= dst_size) {
        // Truncate file first, before truncating dir
        if (dst_size > len_dir) {
            len_file = dst_size - len_dir - 1;
        } else {
            len_file = 0;
            len_dir = dst_size - 1;
        }
    }

    // Concatenate both strings
    memcpy(dst, src, len_dir);
    memcpy(dst + len_dir, src->file, len_file);
    dst[len_dir + len_file] = '\0';
    return dst;
}

// 0x00411b90
__stdcall int string_alloc(size_t size, mwstring *string)
{
    char *data = GlobalAlloc(GMEM_ZEROINIT, size);
    if (!data) {
        string->data = NULL;
        string->size = size;
        return my_GetLastError();
    }
    string->data = data;
    string->size = size;
    return 0;
}

// 0x00411bc0
__stdcall int string_realloc(mwstring *string, size_t size)
{
    char *data = GlobalReAlloc(string->data, size, GMEM_ZEROINIT | GMEM_MOVEABLE);
    if (!data) {
        string->data = NULL;
        string->size = 0;
        return my_GetLastError();
    }
    string->data = data;
    string->size = size;
    return 0;
}

// 0x00411c00
__stdcall char *string_data(mwstring *string)
{
    if (GlobalFlags(string->data) == GMEM_INVALID_HANDLE) {
        return NULL;
    }
    return string->data;
}

// 0x00411c80
__stdcall int string_size(mwstring *string, size_t *out)
{
    if (GlobalFlags(string->data) == GMEM_INVALID_HANDLE) {
        *out = 0;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    *out = string->size;
    return 0;
}

// 0x00417ea0
__stdcall int string_append(mwstring *string, char *data, size_t size)
{
    int rv;

    size_t cur_size;
    rv = string_size(string, &cur_size);
    if (rv) return rv;

    rv = string_realloc(string, cur_size + size);
    if (rv) return rv;

    char *cur_data = string_data(string);
    if (cur_data != NULL) {
        memcpy(cur_data + cur_size, data, size);
    }
    return 0;
}

// 0x00425ac0
__cdecl void *malloc_clear(size_t size)
{
    return GlobalAlloc(GMEM_ZEROINIT, size);
}

// 0x0043c880
__cdecl char *depfile_escape_spaces(int doit, char *dst, char *src)
{
    char *s = src;
    char *d = dst;

    if (!doit) return src;

    while (*s) {
        if (*s == ' ') *d++ = '\\';
        *d++ = *src++;
    }
    *d = '\0';
    return dst;
}

#ifdef SKREW_FIX_DEPFILES
char *relpath(const char *cwd, const char *dst)
{
    const char *dst_p = dst;
    const char *cwd_p = cwd;

#ifndef _WIN32
    static const char sep = '/';
    static const char *sep_parent = "../";
#else
    static const char sep = '\\';
    static const char *sep_parent = "..\\";
    // Strip drive letter if it matches
#define X(x) (x[0] >= 'A' && x[0] <= 'Z' && x[1] == ':' && x[2] == sep)
    if (X(dst_p) && X(cwd_p) && strncmp(dst_p, cwd_p, 3) == 0) {
        dst_p += 2;
        cwd_p += 2;
    }
#undef X
#endif

    // Strip any leading path components
    for (;;) {
        if (*dst_p != sep) break;
        char *c = strchr(dst_p + 1, sep);
        if (!c) break;
        int l = c - dst_p;
        if (strncmp(dst_p, cwd_p, l) != 0) break;
        dst_p += l; cwd_p += l;
    }

    // Figure out how many ../ to add
    int l = 0;
    if (strcmp(dst_p, cwd_p) == 0) {
        dst_p = ".";
    } else if (*dst_p == sep) {
        dst_p++;
        for (; (cwd_p = strchr(cwd_p, sep)); cwd_p++) l++;
    }

    // Allocate and build final string
    int dst_p_len = strlen(dst_p) + 1;
    char *dst_m = malloc(3 * l + dst_p_len);
    if (!dst_m) return NULL;
    for (int i = 0; i < l; i++) memcpy(dst_m + 3 * i, sep_parent, 3);
    memcpy(dst_m + 3 * l, dst_p, dst_p_len);

    return dst_m;
}
#endif

extern const int depfile_struct__source;
extern const int depfile_struct__targets;
extern const int depfile_struct__num_headers;
extern const int depfile_struct__headers;

// 0x0043c8d0
__cdecl void depfile_build(char *header_struct, char *depfile_struct, mwstring *string)
{
    char strbuf[PATH_MAX * 2];
    char escape_buf[PATH_MAX * 2 - 4];

    if (string_alloc(0, string)) goto outofmem;

    int num_headers =
        *(int *)(depfile_struct + depfile_struct__num_headers);

    char target[PATH_MAX];
    depfile_get_target(depfile_struct + depfile_struct__targets,
        NULL, target, PATH_MAX);

    // Print makefile target
    if (!*target) {
        char *source = depfile_struct + depfile_struct__source;
#ifdef SKREW_FIX_DEPFILES
        // Convert to unix path and truncate
        char *source_unx = path_dup_unx(source);
        if (!memccpy(target, source_unx, '\0', sizeof(target))) {
            target[sizeof(target) - 1] = '\0';
        }
        source = target;
        free(source_unx);
#endif
        char *source_escaped = depfile_escape_spaces(
            strchr(source, ' ') != NULL, escape_buf, source);

        sprintf(strbuf, "%s: %s\n", source_escaped, num_headers ? "\\" : "");
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;
    } else {
#ifdef SKREW_FIX_DEPFILES
        // Convert to unix path and truncate
        char *target_unx = path_dup_unx(target);
        if (!memccpy(target, target_unx, '\0', sizeof(target))) {
            target[sizeof(target) - 1] = '\0';
        }
        free(target_unx);
#endif
        char *target_escaped = depfile_escape_spaces(
            strchr(target, ' ') != NULL, escape_buf, target);

        sprintf(strbuf, "%s: ", target_escaped);
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;

        char *source = depfile_struct + depfile_struct__source;
#ifdef SKREW_FIX_DEPFILES
        // Convert to unix path and truncate
        char *source_unx = path_dup_unx(source);
        if (!memccpy(target, source_unx, '\0', sizeof(target))) {
            target[sizeof(target) - 1] = '\0';
        }
        source = target;
        free(source_unx);
#endif
        char *source_escaped = depfile_escape_spaces(
            strchr(source, ' ') != NULL, escape_buf, source);

        sprintf(strbuf, "%s %s\n", source_escaped, num_headers ? "\\" : "");
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;
    }

    // Print all header dependencies
    for (int cur_header = 0; cur_header < num_headers; cur_header++) {
        num_headers--;

        mwpath header;
        char header_full[PATH_MAX];

        depfile_get_header(header_struct, (*(int **)(depfile_struct +
            depfile_struct__headers))[cur_header], &header);
        path_join(&header, header_full, PATH_MAX);

#ifdef SKREW_FIX_DEPFILES
        // Get both paths
        char *cwd = getcwd(NULL, 0);
        if (!cwd) goto outofmem;

        fprintf(stderr, "cwd: %s\n", cwd);
        fprintf(stderr, "header_full: %s\n", header_full);

        // Make relative path, truncate it, and always use forward slashes
        char *rel = relpath(cwd, header_full);
        if (!memccpy(header_full, rel, '\0', sizeof(header_full))) {
            header_full[sizeof(header_full) - 1] = '\0';
        }
        for (char *c = header_full; (c = strchr(c, '\\')); c++) *c = '/';
        free(rel);
        free(cwd);
#endif

        char *header_escaped = depfile_escape_spaces(
            strchr(header_full, ' ') != NULL, escape_buf, header_full);

        sprintf(strbuf, "\t%s %s\n", header_escaped, num_headers ? "\\" : "");
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;
    }
    return;

outofmem:
    fprintf(stderr, "\n*** Out of memory\n");
    exit(233);
}
