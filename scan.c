#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct loc {
    char *name;
    size_t start;
    size_t end;
};

struct file {
    size_t size;
    unsigned char data[];
};

struct scan {
    int offset;
    size_t size;
    unsigned char *data;
};
#define DEF_SCAN(_offset, ...) {\
    .offset = _offset, \
    .data = (unsigned char *)&((unsigned char[]){__VA_ARGS__}), \
    .size = sizeof((unsigned char[]){__VA_ARGS__})}
#define END_SCAN {}

uint16_t read_u16(const unsigned char *mem)
{
    return mem[0] << 0 | mem[1] << 8;
}

uint32_t read_u32(const unsigned char *mem)
{
    return mem[0] << 0 | mem[1] << 8 | mem[2] << 16 | mem[3] << 24;
}

const unsigned char *scan(const struct file *binary, const struct scan *scan)
{
    size_t off = 0;
    for (;;) {
        const struct scan *cur = scan;

        // Find the first scan region
        unsigned char *pos =
            memmem(binary->data + off, binary->size - off,
                cur->data, cur->size);
        if (!pos) return NULL;
        off = pos - binary->data;

        // Make sure the base offset makes sense
        size_t scan_base = off - cur->offset;
        off++;
        if (scan_base > binary->size) continue;

        // Verify that the rest of the scan regions match
        while ((++cur)->data) {
            size_t scan_off = scan_base + cur->offset;
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

    static const char code[] = {
        0x64, 0xa1, 0x00, 0x00, 0x00, 0x00,       // mov eax, fs:[0]
        0x50,                                     // push eax
        0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00  // mov fs:[0], esp
    };

    // Find the chunks
    size_t off = 0;
    while (found < 3) {
        unsigned char *pos =
            memmem(binary->data + off, binary->size - off, code, sizeof(code));
        if (!pos) return found;
        off = pos - binary->data;
        loc[found].start = off;
        loc[found].end = off + sizeof(code);
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
            0x68, 0x00, 0x00, 0x60, 0x00,  // push 0x600000
            0xe8                           // call unk
        ),
        // Verify the instructions for all the parameters we're extracting
        DEF_SCAN(-10,
            0xe8  // call init_args
        ),
        DEF_SCAN(-5,
            0xe8  // call init_envp
        ),
        DEF_SCAN(29,
            0xe8  // call main
        ),
        DEF_SCAN(11,
            0xff, 0x35  // push dword ptr [envp]
        ),
        DEF_SCAN(17,
            0xff, 0x35  // push dword ptr [argv]
        ),
        DEF_SCAN(23,
            0xff, 0x35  // push dword ptr [argc]
        ),
        END_SCAN
    };

    const unsigned char *pos = scan(binary, code);

    static struct loc loc[] = {
        {.name = "init_args"},
        {.name = "init_envp"},
        {.name = "main"},
        {.name = "envp"},
        {.name = "argv"},
        {.name = "argc"}
    };

    if (!pos) return 0;
    loc[0].start = read_u32(pos - 10 + 1);
    loc[1].start = read_u32(pos - 5 + 1);
    loc[2].start = read_u32(pos + 29 + 1);
    loc[3].start = read_u32(pos + 11 + 2);
    loc[4].start = read_u32(pos + 17 + 2);
    loc[5].start = read_u32(pos + 23 + 2);
    *res = loc;
    return 6;
}

typedef unsigned (*funcs_t)(const struct file *, const struct loc **);
static const funcs_t funcs[] = {
    find_fs,
    find_init,
    NULL
};

int scan_bin(FILE *out, const struct file *binary)
{
    funcs_t func;
    for (const funcs_t *p = funcs; (func = *p); p++) {
        const struct loc *patches;
        unsigned len = func(binary, &patches);
        for (unsigned i = 0; i < len; i++) {
            const struct loc *loc = patches + i;
            fprintf(out, "\n");
            if (loc->end) {
                fprintf(out, "code_%s = 0x%lx\n", loc->name, loc->start);
                fprintf(out, "code_%s.end = 0x%lx\n", loc->name, loc->end);
            } else {
                fprintf(out, "addr_%s = 0x%lx\n", loc->name, loc->start);
            }
        }
    }

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

int main(int argc, char *argv[])
{
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <out> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *out = fopen(argv[1], "wb");
    if (!out) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    struct file *binary = file_read(argv[2]);
    if (!binary) {
        fprintf(stderr, "Failed to open file: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    return scan_bin(out, binary);
}
