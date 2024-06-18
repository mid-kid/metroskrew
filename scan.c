#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct patch {
    char *name;
    size_t start;
    size_t end;
};

struct file {
    size_t size;
    unsigned char data[];
};

unsigned find_fs(const struct file *binary, const struct patch **res)
{
    // Near the entrypoint, there's two uses of the "fs" register.
    // This function finds both blocks.

    static struct patch patch[] = {
        {.name = "fs_1"},
        {.name = "fs_2"},
        {.name = "fs_3"}
    };
    unsigned found = 0;
    *res = patch;

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
        patch[found].start = off;
        patch[found].end = off + sizeof(code);
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

typedef unsigned (*funcs_t)(const struct file *, const struct patch **);
static const funcs_t funcs[] = {
    find_fs,
    NULL
};

int scan_bin(FILE *out, const struct file *binary)
{
    funcs_t func;
    for (const funcs_t *p = funcs; (func = *p); p++) {
        const struct patch *patches;
        unsigned len = func(binary, &patches);
        for (unsigned i = 0; i < len; i++) {
            const struct patch *patch = patches + i;
            fprintf(out, "\n");
            fprintf(out, "patch_%s = 0x%lx\n", patch->name, patch->start);
            fprintf(out, "patch_%s.end = 0x%lx\n", patch->name, patch->end);
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
