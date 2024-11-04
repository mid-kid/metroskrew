#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include.h"

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

// 0x0043c8d0
__cdecl void depfile_build(char *header_struct, char *depfile_struct, mwstring *string)
{
    char strbuf[PATH_MAX * 2];
    char escape_buf[PATH_MAX * 2 - 4];

    if (string_alloc(0, string)) goto outofmem;

    int num_headers = *(int *)(depfile_struct + 0x870);

    char target[PATH_MAX];
    depfile_get_target(depfile_struct + 0x423, NULL, target, PATH_MAX);

    // Print makefile target
    if (!*target) {
        char *source = depfile_struct + 0x1c;
        char *source_escaped = depfile_escape_spaces(
            strchr(source, ' ') != NULL, escape_buf, source);

        sprintf(strbuf, "%s: %s\n", source_escaped, num_headers ? "\\" : "");
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;
    } else {
        char *target_escaped = depfile_escape_spaces(
            strchr(target, ' ') != NULL, escape_buf, target);

        sprintf(strbuf, "%s: ", target_escaped);
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;

        char *source = depfile_struct + 0x1c;
        char *source_escaped = depfile_escape_spaces(
            strchr(source, ' ') != NULL, escape_buf, source);

        sprintf(strbuf, "%s %s\n", source_escaped, num_headers ? "\\" : "");
        if (string_append(string, strbuf, strlen(strbuf))) goto outofmem;
    }

    // Print all header dependencies
    for (int cur_header = 0; cur_header < *(int *)(depfile_struct + 0x870);
            cur_header++) {
        num_headers--;

        mwpath header;
        char header_full[PATH_MAX];

        depfile_get_header(header_struct,
            (*(int **)(depfile_struct + 0x878))[cur_header], &header);
        path_join(&header, header_full, PATH_MAX);

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
