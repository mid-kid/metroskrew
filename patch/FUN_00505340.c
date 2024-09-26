#include <stddef.h>
#include <stdint.h>

// This function mallocs some memory without clearing it, and reads from it,
// which causes the behavior (and compilation output) to depend on it.
//
// Unfortunately, the reallocated memory contains pointers, which differ
// depending on the OS and ASLR. This code tries to rectify it.

struct STRUC_0063a828 {
    struct STRUC_0063a828 *next;
    int _unk1[1];
    struct STRUC_0063a828 *unk_8;
    struct STRUC_0063a828 *unk_c;
    int _unk2[3];
    int unk_1c;
};

extern int DAT_0063a798;  // 0x0063a798
extern struct STRUC_0063a828 *DAT_0063a828;
extern struct STRUC_0063a828 **DAT_0063ccb0;
extern uint32_t **DAT_0063ccf0;  // 0x0063ccf0

void *prog_malloc(size_t size);  // 0x00442550
void FUN_004f8b60(void);
void bitarr_cpy(uint32_t *dst, uint32_t *src, int len);  // 0x00581750
int bitarr_cpycmp(uint32_t *dst, uint32_t *src, int len);  // 0x00581790
void bitarr_set(uint32_t *dst, int len, uint32_t val);  // 0x005817d0
void bitarr_and(uint32_t *dst, uint32_t *src, int len);  // 0x005818f0

// 0x00505340
void FUN_00505340(void)
{
    int wordlen = (DAT_0063a798 + 31) / 32;

    DAT_0063ccf0 = prog_malloc(DAT_0063a798 * sizeof(*DAT_0063ccf0));
    for (int i = 0; i < DAT_0063a798; i++) {
        DAT_0063ccf0[i] = prog_malloc(wordlen * sizeof(**DAT_0063ccf0));
    }

    uint32_t *curbits = prog_malloc(wordlen * sizeof(**DAT_0063ccf0));

    bitarr_set(DAT_0063ccf0[DAT_0063a828->unk_1c], DAT_0063a798, 0);
    DAT_0063ccf0[DAT_0063a828->unk_1c][0] |= 1;

    for (struct STRUC_0063a828 *listptr = DAT_0063a828->next;
            listptr; listptr = listptr->next) {
        bitarr_set(DAT_0063ccf0[listptr->unk_1c], DAT_0063a798, -1);
    }

    FUN_004f8b60();

    for (;;) {
        int cont = 0;
        for (int i = 0; i < DAT_0063a798; i++) {
            struct STRUC_0063a828 *cur1 = DAT_0063ccb0[i];
            if (!cur1) continue;
            if (cur1->unk_1c == DAT_0063a828->unk_1c) continue;

            struct STRUC_0063a828 *cur2 = cur1->unk_c;
            bitarr_cpy(curbits, DAT_0063ccf0[cur2->unk_c->unk_1c], DAT_0063a798);
            for (cur2 = cur2->unk_8; cur2; cur2 = cur2->unk_8) {
                bitarr_and(curbits, DAT_0063ccf0[cur2->unk_c->unk_1c], DAT_0063a798);
            }
            curbits[cur1->unk_1c >> 5] |= 1 << (cur1->unk_1c & 0x1f);

            cont += bitarr_cpycmp(DAT_0063ccf0[cur1->unk_1c], curbits, DAT_0063a798);
        }
        if (!cont) break;
    }
}

// 0x00581750
void bitarr_cpy(uint32_t *dst, uint32_t *src, int len)
{
    for (int i = 0; i < (len + 31) / 32; i++) {
        *dst++ = *src++;
    }
}

// 0x00581790
int bitarr_cpycmp(uint32_t *dst, uint32_t *src, int len)
{
    int res = 0;
    for (int i = 0; i < (len + 31) / 32; i++) {
        if (*dst != *src) res = 1;
        *dst++ = *src++;
    }
    return res;
}

// 0x005817d0
void bitarr_set(uint32_t *dst, int len, uint32_t val)
{
    for (int i = 0; i < (len + 31) / 32; i++) {
        *dst++ = val;
    }
}

// 0x005818f0
void bitarr_and(uint32_t *dst, uint32_t *src, int len)
{
    for (int i = 0; i < (len + 31) / 32; i++) {
        *dst++ &= *src++;
    }
}
