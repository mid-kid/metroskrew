#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The program mallocs some memory without clearing it, and reads from it,
// which causes the behavior (and compilation output) to depend on it.
//
// Unfortunately, the reallocated memory contains pointers, which differ
// depending on the OS and ASLR. This code tries to rectify it.
//
// The hook is placed right after the malloc() happens for all the elements of
// the array, allowing us to fix the values manually.

extern unsigned int **memreuse01_arr;
extern unsigned int memreuse01_len;

void patch_memreuse01(void)
{
    unsigned int elemlen = (memreuse01_len + 0x1f) >> 5;
    (void)elemlen;

#if 0
    for (unsigned y = 0; y < memreuse01_len; y++) {
        memset(memreuse01_arr[y], 0, elemlen * sizeof(unsigned int));
        //memset(memreuse01_arr[y], -1, elemlen * sizeof(unsigned int));
    }
#endif

#if 0
    if (getenv("SKREW_HACK_MEMREUSE01_DUMP")) {
        for (unsigned y = 0; y < memreuse01_len; y++) {
            fprintf(stderr, "memreuse01_arr[%02d]: ", y);
            for (unsigned x = 0; x < elemlen; x++) {
                fprintf(stderr, "%08x", memreuse01_arr[y][x]);
            }
            fprintf(stderr, "\n");
            fflush(stderr);
        }
    }
#endif
}

void patch_memreuse01_exit(void)
{
    unsigned int elemlen = (memreuse01_len + 0x1f) >> 5;
    (void)elemlen;

#if 1
    if (getenv("SKREW_HACK_MEMREUSE01_DUMP")) {
        for (unsigned y = 0; y < memreuse01_len; y++) {
            fprintf(stderr, "memreuse01_arr[%02d]: ", y);
            for (unsigned x = 0; x < elemlen; x++) {
                fprintf(stderr, "%08x", memreuse01_arr[y][x]);
            }
            fprintf(stderr, "\n");
            fflush(stderr);
        }
    }
#endif
}
