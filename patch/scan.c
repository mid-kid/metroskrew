#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct {
    char *out;
    char *incbin;
} opt;

struct loc {
    char *name;
    size_t start;
    size_t end;
    bool raw;
};

struct file {
    size_t size;
    unsigned char data[];
};

struct scan {
    int off;
    size_t size;
    unsigned char *data;
};
#define DEF_SCAN(_off, ...) { \
    .off = _off, \
    .data = (unsigned char *)&((unsigned char[]){__VA_ARGS__}), \
    .size = sizeof((unsigned char[]){__VA_ARGS__})}
#define END_SCAN {}

uint16_t read_u16(const unsigned char *mem)
{
    return (uint16_t)mem[0] << 0 | (uint16_t)mem[1] << 8;
}

uint32_t read_u32(const unsigned char *mem)
{
    return (uint32_t)mem[0] << 0 | (uint32_t)mem[1] << 8 |
        (uint32_t)mem[2] << 16 | (uint32_t)mem[3] << 24;
}

const unsigned char *scan(const struct file *binary, const struct scan *scan, size_t off)
{
    for (;;) {
        const struct scan *cur = scan;

        // Find the first scan region
        unsigned char *pos =
            memmem(binary->data + off, binary->size - off,
                cur->data, cur->size);
        if (!pos) return NULL;
        off = pos - binary->data;

        // Make sure the base offset makes sense
        size_t scan_base = off - cur->off;
        off++;
        if (scan_base > binary->size) continue;

        // Verify that the rest of the scan regions match
        while ((++cur)->data) {
            size_t scan_off = scan_base + cur->off;
            if (scan_off > binary->size) break;
            if (scan_off + cur->size > binary->size) break;
            if (memcmp(binary->data + scan_off, cur->data, cur->size) != 0) break;
        }
        if (cur->data) continue;
        return binary->data + scan_base;
    }
}

unsigned find_fs(const struct file *binary, const struct loc **res)
{
    // Near the entrypoint, there's two uses of the "fs" register.
    // This function finds both blocks.

    static struct loc loc[] = {
        {.name = "fs_1"},
        {.name = "fs_2"},
        {.name = "fs_3"}
    };
    unsigned found = 0;
    *res = loc;

    const struct scan code[] = {
        DEF_SCAN(0,
            0x64, 0xa1, 0x00, 0x00, 0x00, 0x00,       // mov eax, fs:[0]
            0x50,                                     // push eax
            0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00  // mov fs:[0], esp
        ),
        END_SCAN
    };

    // Find the chunks
    size_t off = 0;
    while (found < 3) {
        const unsigned char *pos = scan(binary, code, off);
        if (!pos) return found;
        off = pos - binary->data;
        loc[found].start = off;
        loc[found].end = off + code[0].size;
        found++;
        off++;
    }

    // Make sure we got everything
    unsigned char *left = 
        memmem(binary->data + off, binary->size - off, code, sizeof(code));
    if (left) {
        size_t left_off = left - binary->data;
        fprintf(stderr, "warning: leftover match of %s at 0x%lx\n", __func__,
            left_off);
    }

    return found;
}

unsigned find_init(const struct file *binary, const struct loc **res)
{
    // Find the entrypoint, and extract the locations of a few functions
    //  as well as addresses that can be found there.

    const struct scan code[] = {
        // Sentinel code
        DEF_SCAN(0,
            0x85, 0xc0,  // test eax, eax
            0x75, 0x08,  // jne +10
            0x6a, 0xff,  // push -1
            0xe8,        // call unk
        ),
        // Verify the instructions for all the parameters we're extracting
        DEF_SCAN(33,
            0xff, 0x35  // push dword ptr [envp]
        ),
        DEF_SCAN(39,
            0xff, 0x35  // push dword ptr [argv]
        ),
        DEF_SCAN(45,
            0xff, 0x35  // push dword ptr [argc]
        ),
        DEF_SCAN(12,
            0xe8  // call init_args
        ),
        DEF_SCAN(17,
            0xe8  // call init_envp
        ),
        DEF_SCAN(51,
            0xe8  // call main
        ),
        END_SCAN
    };

    static struct loc loc[] = {
        {.name = "envp"},
        {.name = "argv"},
        {.name = "argc"},
        {.name = "init_args"},
        {.name = "init_envp"},
        {.name = "main"}
    };

    const unsigned char *pos = scan(binary, code, 0);
    if (!pos) return 0;
    size_t off = pos - binary->data;

    loc[0].start = read_u32(pos + code[1].off + code[1].size);
    loc[1].start = read_u32(pos + code[2].off + code[2].size);
    loc[2].start = read_u32(pos + code[3].off + code[3].size);

    const unsigned char *end;
    static const unsigned char end_init_args[] = {
        0x89, 0xf2,       // mov edx, esi
        0x2b, 0x55, 0xec  // sub edx, dword ptr [ebp - 0x14]
    };
    static const unsigned char end_init_envp[] = {
        0x5b,  // pop ebx
        0xc3   // ret
    };

    loc[3].start = read_u32(pos + code[4].off + code[4].size) +
        off + code[4].off + 5;
    loc[3].end = 0;
    end = memmem(binary->data + loc[3].start, binary->size - loc[3].start,
        end_init_args, sizeof(end_init_args));
    if (end) {
        loc[3].end = end - binary->data + sizeof(end_init_args) + 11;
    }

    loc[4].start = read_u32(pos + code[5].off + code[5].size) +
        off + code[5].off + 5;
    loc[4].end = 0;
    end = memmem(binary->data + loc[4].start, binary->size - loc[4].start,
        end_init_envp, sizeof(end_init_envp));
    if (end) {
        size_t off = end - binary->data + sizeof(end_init_envp);
        while (off < binary->size && binary->data[off] == 0x90) off++;
        loc[4].end = off;
    }

    loc[5].start = read_u32(pos + code[6].off + code[6].size) +
        off + code[6].off + 5;
    loc[5].end = loc[5].start;  // Not relevant for now

    *res = loc;
    return 6;
}

unsigned find_getenv(const struct file *binary, const struct loc **res)
{
    // Find the getenv() function

    const struct scan code[] = {
        DEF_SCAN(0,
            0x31, 0xc0,       // xor eax, eax
            0x56,             // push esi
            0x57,             // push edi
            0x83, 0xec, 0x04  // sub esp, 4
        ),
        END_SCAN
    };

    static struct loc loc[] = {
        {.name = "getenv"}
    };

    const unsigned char *pos = scan(binary, code, 0);
    if (!pos) return 0;
    size_t off = pos - binary->data;

    const unsigned char *end;
    static const unsigned char end_code[] = {
        0x31, 0xc0,        // xor eax, eax
        0x83, 0xc4, 0x04,  // add esp, 4
        0x5f,              // pop edi
        0x5e,              // pop esi
        0xc3               // ret
    };

    loc[0].start = off;
    loc[0].end = 0;
    end = memmem(binary->data + loc[0].start, binary->size - loc[0].start,
        end_code, sizeof(end_code));
    if (end) {
        size_t off = end - binary->data + sizeof(end_code);
        while (off < binary->size && binary->data[off] == 0x90) off++;
        loc[0].end = off;
    }

    *res = loc;
    return 1;
}

unsigned find_findexe(const struct file *binary, const struct loc **res)
{
    const struct scan code[] = {
        DEF_SCAN(0,
            0x57,                                     // push edi
            0x55,                                     // push ebp
            0x81, 0xec, 0x08, 0x02, 0x00, 0x00,       // sub esp, 0x208
            0x8b, 0xac, 0x24, 0x18, 0x02, 0x00, 0x00  // mov ebp, dword [esp + 0x218]
        ),
        END_SCAN
    };

    static struct loc loc[] = {
        {.name = "findexe"}
    };

    const unsigned char *pos = scan(binary, code, 0);
    if (!pos) return 0;
    size_t off = pos - binary->data;

    const unsigned char *end;
    static const unsigned char end_code[] = {
        0x90, 0x90  // nop nop
    };

    loc[0].start = off;
    loc[0].end = 0;
    end = memmem(binary->data + loc[0].start, binary->size - loc[0].start,
        end_code, sizeof(end_code));
    if (end) {
        size_t off = end - binary->data;
        while (off < binary->size && binary->data[off] == 0x90) off++;
        loc[0].end = off;
    }

    *res = loc;
    return 1;
}

unsigned find_memreuse01(const struct file *binary, const struct loc **res)
{
    // Find the code that allocates some memory without clearing it
    // This allows us to stably control UB in the codegen

    const struct scan code[] = {
        DEF_SCAN(0,
            0x53,        // push ebx
            0x8b, 0x1d,  // mov ebx, dword ptr [u32]
        ),
        DEF_SCAN(23,
            0xe8  // call .+u32
        ),
        DEF_SCAN(28,
            0xa3  // mov [u32], eax
        ),
        DEF_SCAN(117,
            0x8b, 0x2d  // mov ebp, dword ptr [u32]
        ),
        DEF_SCAN(205,
            0xe8  // call .+u32
        ),
        DEF_SCAN(240,
            0x8b, 0x15  // mov edx, dword ptr
        ),
        DEF_SCAN(417,
            0xc7, 0x44, 0x24, 0x04, 0x01, 0x00, 0x00, 0x00,  // mov dword ptr [esp + 4], 1
            0xeb, 0xa9,                                      // jmp -85
            0x90, 0x90, 0x90, 0x90, 0x90
        ),
        END_SCAN
    };

    static struct loc loc[] = {
        {.name = "FUN_00505340"},
        {.name = "DAT_0063a798"},
        {.name = "DAT_0063ccf0"},
        {.name = "DAT_0063a828"},
        {.name = "DAT_0063ccb0"},
        {.name = "prog_malloc"},
        {.name = "FUN_004f8b60"}
    };

    const unsigned char *pos = scan(binary, code, 0);
    if (!pos) return 0;
    size_t off = pos - binary->data;

    loc[0].start = off + code[0].off;
    loc[0].end = off + code[6].off + code[6].size;
    loc[1].start = read_u32(pos + code[0].off + code[0].size);
    loc[2].start = read_u32(pos + code[2].off + code[2].size);
    loc[3].start = read_u32(pos + code[3].off + code[3].size);
    loc[4].start = read_u32(pos + code[5].off + code[5].size);

    loc[5].start = off + code[1].off + code[1].size + 4 +
        (int32_t)read_u32(pos + code[1].off + code[1].size);
    loc[5].end = loc[5].start;
    loc[6].start = off + code[4].off + code[4].size + 4 +
        (int32_t)read_u32(pos + code[4].off + code[4].size);
    loc[6].end = loc[6].start;

    *res = loc;
    return 7;
}

unsigned find_depfile(const struct file *binary, const struct loc **res)
{
    const struct scan code[] = {
        DEF_SCAN(0,
            0x53,                                     // push ebx
            0x56,                                     // push esi
            0x57,                                     // push edi
            0x55,                                     // push ebp
            0x81, 0xec, 0x1c, 0x08, 0x00, 0x00,       // sub esp, 0x81c
            0x8b, 0x9c, 0x24, 0x34, 0x08, 0x00, 0x00  // mov ebx, dword ptr [esp + u32]
        ),
        // TODO: Find real end
        END_SCAN
    };

    const struct scan code_get_target[] = {
        DEF_SCAN(0,
            0x53,                                // push ebx
            0x56,                                // push esi
            0x57,                                // push edi
            0x55,                                // push ebp
            0x81, 0xec, 0x0c, 0x03, 0x00, 0x00,  // sub esp, 0x30c
            0x8d, 0x9c, 0x24                     // lea ebx, [esp + u32]
        ),
        // TODO: Find real end
        END_SCAN
    };

    const struct scan code_get_header[] = {
        DEF_SCAN(0,
            0x8b, 0x4c, 0x24, 0x04,                   // mov ecx, dword ptr [esp + 4]
            0x8b, 0x44, 0x24, 0x08,                   // mov eax, dword ptr [esp + 8]
            0x8d, 0x04, 0x80,                         // lea eax, [eax + eax * 4]
            0x8d, 0x04, 0x85, 0x00, 0x00, 0x00, 0x00  // lea eax, [eax * 4]
        ),
        // TODO: Find real end
        END_SCAN
    };

    static struct loc loc[] = {
        {.name = "depfile_build"},
        {.name = "depfile_get_target"},
        {.name = "depfile_get_header"},
        {.name = "off_depfile_struct__source", .raw = true},
        {.name = "off_depfile_struct__targets", .raw = true},
        {.name = "off_depfile_struct__num_headers", .raw = true},
        {.name = "off_depfile_struct__headers", .raw = true}
    };

    const unsigned char *pos = scan(binary, code, 0);
    if (!pos) return 0;
    size_t off = pos - binary->data;

    loc[0].start = off + code[0].off;
    loc[0].end = loc[0].start + code[0].size;

    pos = scan(binary, code_get_target, 0);
    if (!pos) return 0;
    off = pos - binary->data;

    loc[1].start = off + code_get_target[0].off;
    loc[1].end = loc[1].start + code_get_target[0].size;

    pos = scan(binary, code_get_header, 0);
    if (!pos) return 0;
    off = pos - binary->data;

    loc[2].start = off + code_get_header[0].off;
    loc[2].end = loc[2].start + code_get_header[0].size;

    // Figure out what version of the struct it's using
    const unsigned char *push;
    static const unsigned char push_code[] = {
        0x68, 0x04, 0x01, 0x00, 0x00  // push 0x104
    };
    if (loc[0].start < 4) return 0;
    if (binary->size - loc[0].start < 0x80) return 0;
    push = memmem(binary->data + loc[0].start, 0x80, push_code, sizeof(push_code));
    if (!push) return 0;
    uint32_t struct_off = read_u32(push - 4);

    // Hardcode the values for known versions
    switch (struct_off) {
    case 0x870:
        loc[3].start = 0x1c;  // source
        loc[4].start = 0x423;  // targets
        loc[5].start = 0x870;  // num_headers
        loc[6].start = 0x878;  // headers
        break;
    case 0x86c:
        loc[3].start = 0x1c;  // source
        loc[4].start = 0x422;  // targets
        loc[5].start = 0x86c;  // num_headers
        loc[6].start = 0x874;  // headers
        break;
    default: return 0;
    }

    *res = loc;
    return 7;
}

typedef unsigned (*funcs_t)(const struct file *, const struct loc **);
static const funcs_t funcs[] = {
    find_fs,
    find_init,
    find_getenv,
    find_findexe,
    find_memreuse01,
    find_depfile,
    NULL
};

int sort_loc(const void *_p1, const void *_p2)
{
    const struct loc *p1 = _p1, *p2 = _p2;
    if (p2->raw != p1->raw) return p2->raw - p1->raw;
    return p1->start - p2->start;
}

int scan_bin(FILE *out, const struct file *binary, const char *incbin)
{
    struct loc *patches = NULL;
    unsigned patches_len = 0;

    // Find and sort all locations
    funcs_t func;
    for (const funcs_t *p = funcs; (func = *p); p++) {
        const struct loc *patches_cur;
        unsigned len = func(binary, &patches_cur);
        patches = realloc(patches, sizeof(*patches) * (patches_len + len));
        memcpy(patches + patches_len, patches_cur, sizeof(*patches) * len);
        patches_len += len;
    }
    qsort(patches, patches_len, sizeof(*patches), sort_loc);

    // Generate header
    fprintf(out,
        ".macro incbin off, len\n"
        ".incbin \"%s\", \\off, \\len\n"
        ".endm\n",
        incbin
    );

    // pe_text_off and pe_text_len need to be defined here, since .fill
    //  requires constant values...
    const struct scan text_code[] = {
        DEF_SCAN(0,
            '.', 't', 'e', 'x', 't', 0, 0, 0
        ),
        DEF_SCAN(36,
            0x20, 0x00, 0x00, 0x60
        ),
        END_SCAN
    };
    const unsigned char *text_pos = scan(binary, text_code, 0);
    uint32_t pe_text_off = 0;
    uint32_t pe_text_len = 0;
    if (text_pos) {
        pe_text_off = read_u32(text_pos + 20);
        pe_text_len = read_u32(text_pos + 8);
        uint32_t pe_base_addr = read_u32(binary->data + 0xb4);
        uint32_t pe_text_addr = pe_base_addr + read_u32(text_pos + 12);
        fprintf(out, "\n"
            "pe_text_off = 0x%x\n"
            "pe_text_len = 0x%x\n"
            "pe_text_addr = 0x%x\n",
            pe_text_off, pe_text_len, pe_text_addr
        );
    }

    // Print out all the scan results
    for (unsigned i = 0; i < patches_len; i++) {
        const struct loc *loc = patches + i;
        fprintf(out, "\n");
        if (loc->end) {
            fprintf(out, "code_%s = 0x%lx\n", loc->name, loc->start);
            if (loc->end != loc->start) {
                fprintf(out, "code_%s.end = 0x%lx\n", loc->name, loc->end);
            }
            if (loc->start >= pe_text_off &&
                    loc->start < pe_text_off + pe_text_len &&
                    loc->end >= pe_text_off &&
                    loc->end < pe_text_off + pe_text_len) {
                fprintf(out, "addr_%s = code_%s - pe_text_off + pe_text_addr\n",
                    loc->name, loc->name);
            }
        } else if (!loc->raw) {
            fprintf(out, "addr_%s = 0x%lx\n", loc->name, loc->start);
        } else {
            fprintf(out, "%s = 0x%lx\n", loc->name, loc->start);
        }
    }

    // Generate footer
    fprintf(out, "\n"
        ".include \"patch.i\"\n"
    );

    return EXIT_SUCCESS;
}

struct file *file_read(const char *name)
{
    FILE *f = fopen(name, "rb");
    if (!f) return NULL;

    long size;
    if (fseek(f, 0, SEEK_END) == -1 ||
            (size = ftell(f)) == -1 ||
            fseek(f, 0, SEEK_SET) == -1) {
        fclose(f);
        return NULL;
    }

    struct file *file = malloc(sizeof(struct file) + size);
    if (!file) {
        fclose(f);
        return NULL;
    }
    file->size = size;

    if (fread(file->data, file->size, 1, f) != 1) {
        free(file);
        fclose(f);
        return NULL;
    }
    fclose(f);
    return file;
}

char *main_progname;

void main_usage(void)
{
    fprintf(stderr, "Usage: %s <file>\n", main_progname);
}

int main(int argc, char *argv[])
{
    main_progname = argv[0];

    while (argc > 1) {
        if (*argv[1] != '-') {
            break;
        } else if (strcmp(argv[1], "--") == 0) {
            argv++; argc--;
            break;
        } else if (strcmp(argv[1], "-o") == 0) {
            if (argc <= 1) { main_usage(); return EXIT_FAILURE; }
            opt.out = argv[2];
            argv++; argc--;
        } else if (strcmp(argv[1], "--incbin") == 0) {
            if (argc <= 1) { main_usage(); return EXIT_FAILURE; }
            opt.incbin = argv[2];
            argv++; argc--;
        }
        argv++; argc--;
    }

    if (argc <= 1) {
        main_usage();
        return EXIT_FAILURE;
    }

    struct file *binary = file_read(argv[1]);
    if (!binary) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE *out = stdout;
    if (opt.out) {
        out = fopen(opt.out, "w");
        if (!out) {
            fprintf(stderr, "Failed to open file: %s\n", opt.out);
            return EXIT_FAILURE;
        }
    }

    char *incbin = argv[1];
    if (opt.incbin) incbin = opt.incbin;

    return scan_bin(out, binary, incbin);
}
