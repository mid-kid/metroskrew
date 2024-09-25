#include <stdint.h>
#include <stdio.h>

extern uint8_t memreuse01_arr[];
extern uint32_t memreuse01_len;

void patch_memreuse01(void)
{
    fprintf(stderr, "memreuse01_arr: %p\n", memreuse01_arr);
    fprintf(stderr, "memreuse01_len: 0x%x\n", memreuse01_len);
    fflush(stderr);
}
