#pragma once

#include <stdint.h>

// WINE headers
#include "windef.h"
#include "winbase.h"
DWORD WINAPI my_GetLastError(void);

#define PATH_MAX 0x104

struct STRUC_0063a828 {
    struct STRUC_0063a828 *next;
    int _unk1[1];
    struct STRUC_0063a828 *unk_8;
    struct STRUC_0063a828 *unk_c;
    int _unk2[3];
    int unk_1c;
};

typedef struct {
    char *data;
    unsigned size;
} mwstring;

typedef struct {
    char dir[PATH_MAX];
    char file[PATH_MAX - 4];
} mwpath;

extern int DAT_0063a798;  // 0x0063a798
extern struct STRUC_0063a828 *DAT_0063a828;  // 0x0063a828
extern struct STRUC_0063a828 **DAT_0063ccb0;  // 0x0063ccb0
extern uint32_t **DAT_0063ccf0;  // 0x0063ccf0

__cdecl void *prog_malloc(size_t size);  // 0x00442550
__cdecl void FUN_004f8b60(void);  // 0x004f8b60
