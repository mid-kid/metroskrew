#include <stdio.h>
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
    size_t bitlen = ((memreuse01_len + 0x1f) >> 5) * sizeof(unsigned int);
    (void)bitlen;

    fprintf(stderr, "memreuse01_arr: 0x%lx\n", (long)memreuse01_arr);
    fprintf(stderr, "memreuse01_len: 0x%lx\n", (long)memreuse01_len);

    for (unsigned i = 0; i < memreuse01_len; i++) {
        fprintf(stderr, "memreuse01_arr[i]: 0x%lx\n", (long)memreuse01_arr[i]);
        //memset(memreuse01_arr[i], 0, bitlen);
    }

    fflush(stderr);
}
