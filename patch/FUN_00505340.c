#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// This function mallocs some memory without clearing it, and reads from it,
// which causes the behavior (and compilation output) to depend on it.
//
// Unfortunately, the reallocated memory contains pointers, which differ
// depending on the OS and ASLR. This code tries to allow control over it.

#define SKREW_HACK01

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

    // Keep track of which entries have been initialized
    uint8_t _init[DAT_0063a798]; (void)_init;
    memset(_init, 0, DAT_0063a798);

    _init[DAT_0063a828->unk_1c] = 1;
    bitarr_set(DAT_0063ccf0[DAT_0063a828->unk_1c], DAT_0063a798, 0);
    DAT_0063ccf0[DAT_0063a828->unk_1c][0] |= 1;

    for (struct STRUC_0063a828 *listptr = DAT_0063a828->next;
            listptr; listptr = listptr->next) {
        _init[listptr->unk_1c] = 1;
        bitarr_set(DAT_0063ccf0[listptr->unk_1c], DAT_0063a798, -1);
    }

#ifdef SKREW_HACK01
    static char *hack_chk = (char *)-1;
    if (hack_chk == (char *)-1) hack_chk = getenv("SKREW_HACK_CHK");
    static char *hack_data = (char *)-1;
    if (hack_data == (char *)-1) hack_data = getenv("SKREW_HACK01");
    static char *hack_dbg = (char *)-1;
    if (hack_dbg == (char *)-1) hack_dbg = getenv("SKREW_HACK01_DBG");

    if (!hack_data && hack_chk) {
        // Report if any entries haven't been initialized
        int i = 0;
        for (; i < DAT_0063a798; i++) if (!_init[i]) break;
        if (i != DAT_0063a798) {
            fprintf(stderr, "metroskrew: Non-determinism detected!\n");

            fprintf(stderr, "    data: ");
            for (int y = 0; y < DAT_0063a798; y++) {
                if (_init[y]) continue;
                for (int x = 0; x < wordlen; x++) {
                    uint32_t a = DAT_0063ccf0[y][x];

                    uint32_t b = 0;
                    for (int z = 0; z < 32; z++) {
                        b = (b << 1) | (a & 1);
                        a >>= 1;
                    }

                    fprintf(stderr, "%08x", b);
                }
            }

            fprintf(stderr, "\n"
                "    Use SKREW_HACK01 to set the desired data.\n");
            fflush(stderr);
        }
    }

    if (hack_data) {
        // Restore values passed by the user
        for (int y = 0; y < DAT_0063a798; y++) {
            if (_init[y]) continue;
            for (int x = 0; x < wordlen; x++) {
                uint32_t a = 0;
                int i = 0;
                while (*hack_data && i < 8) {
                    uint8_t c = *hack_data++;
                    uint8_t v = 0;
                    if (c >= '0' && c <= '9') {
                        v = c - '0';
                    } else if (c >= 'A' && c <= 'F') {
                        v = c - 'A' + 10;
                    } else if (c >= 'a' && c <= 'f') {
                        v = c - 'a' + 10;
                    } else {
                        continue;
                    }
                    a = (a << 4) | v;
                    i++;
                }
                if (i == 0) continue;
                a <<= 32 - i * 4;

                uint32_t b = 0;
                for (int z = 0; z < 32; z++) {
                    b = (b << 1) | (a & 1);
                    a >>= 1;
                }

                DAT_0063ccf0[y][x] = b;
            }
        }
    }
#endif

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

#ifdef SKREW_HACK01
    if (hack_dbg) {
        for (int y = 0; y < DAT_0063a798; y++) {
            fprintf(stderr, "arr[%02d]: ", y);
            for (int x = 0; x < wordlen; x++) {
                uint32_t a = DAT_0063ccf0[y][x];
                uint32_t b = 0;
                for (int z = 0; z < 32; z++) {
                    b = (b << 1) | (a & 1);
                    a >>= 1;
                }
                fprintf(stderr, "%08x", b);
            }
            fprintf(stderr, "\n");
            fflush(stderr);
        }
    }
#endif
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
