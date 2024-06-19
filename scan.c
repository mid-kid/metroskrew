#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct loc {
    char *name;
    size_t start;
    size_t end;
};

struct file {
    size_t size;
    unsigned char data[];
};

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

    static struct loc loc[] = {
        {.name = "init_args"},
        {.name = "init_envp"},
        {.name = "main"},
        {.name = "argc"},
        {.name = "argv"},
        {.name = "envp"}
    };

    static const char code[] = {
        0x68, 0x00, 0x00, 0x60, 0x00,  // push 0x600000
        0xe8                           // call [i32]
    };

    for (;;) {
        unsigned char *pos =
            memmem(binary->data, binary->size, code, sizeof(code));
        if (!pos) return 0;
        size_t off = pos - binary->data;
        if (off < 10) return 0;
    }
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
                fprintf(out, "patch_%s = 0x%lx\n", loc->name, loc->start);
                fprintf(out, "patch_%s.end = 0x%lx\n", loc->name, loc->end);
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
