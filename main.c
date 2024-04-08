#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pe_file {
    FILE *f;
    unsigned long header_off;
    unsigned datadir_off;
    unsigned sections_off;
    unsigned sections_num;
    unsigned long entrypoint;
    unsigned long base_addr;
    unsigned long datadir_num;
};

struct pe_section {
    char name[8 + 1];
    unsigned long vsize;
    unsigned long address;
    unsigned long dsize;
    unsigned long offset;
};

struct pe_export {
    unsigned long dllname;
    unsigned long func_num;
    unsigned long name_num;
    unsigned long *func_off;
    unsigned long *name_off;
    unsigned long *ords;
};

struct pe_import {
    unsigned long addr_num;
    unsigned long name_num;
    unsigned long *addr_off;
    unsigned long *dllname_off;
    unsigned long *name_off;
};

uint16_t read_u16(const unsigned char *mem)
{
    return mem[0] << 0 | mem[1] << 8;
}

uint32_t read_u32(const unsigned char *mem)
{
    return mem[0] << 0 | mem[1] << 8 | mem[2] << 16 | mem[3] << 24;
}

bool pe_open_read_header(struct pe_file *file)
{
    unsigned char header_off_b[4];
    if (fseek(file->f, 0x3c, SEEK_SET) == -1) return false;
    if (fread(header_off_b, 4, 1, file->f) != 1) return false;
    file->header_off = read_u32(header_off_b);

    if (fseek(file->f, file->header_off, SEEK_SET) == -1) return false;

    unsigned char header[0x18];
    if (fread(header, sizeof(header), 1, file->f) != 1) return false;
    unsigned char magic[] = {'P', 'E', 0, 0};
    if (memcmp(header, magic, sizeof(magic)) != 0) return false;
    unsigned char opt_header_size = 0x78;
    unsigned char opt_size = read_u16(header + 0x14);

    file->datadir_off = file->header_off + opt_header_size;
    file->sections_off = file->header_off + sizeof(header) + opt_size;
    file->sections_num = read_u16(header + 0x06);

    unsigned char opt_header[opt_header_size];
    if (fread(opt_header, opt_header_size, 1, file->f) != 1) return false;
    if (read_u16(opt_header) != 0x010b) return false;

    file->entrypoint = read_u32(opt_header + 0x10);
    file->base_addr = read_u32(opt_header + 0x1c);
    file->datadir_num = read_u32(opt_header + 0x5c);

    return true;
}

struct pe_file *pe_open(const char *fname)
{
    struct pe_file *file = malloc(sizeof(struct pe_file));

    file->f = fopen(fname, "rb");
    if (!file->f) {
        free(file);
        return NULL;
    }

    if (!pe_open_read_header(file)) {
        free(file);
        return NULL;
    }

    return file;
}

unsigned pe_get_sections_num(struct pe_file *file)
{
    return file->sections_num;
}

unsigned long pe_get_entrypoint(struct pe_file *file)
{
    return file->base_addr + file->entrypoint;
}

bool pe_get_datadir(struct pe_file *file, unsigned num, unsigned long *addr, unsigned long *size)
{
    if (file->datadir_num <= num) return false;

    unsigned datadir_off = file->datadir_off + 8 * num;
    if (fseek(file->f, datadir_off, SEEK_SET) == -1) return false;

    unsigned char entry[8];
    if (fread(entry, sizeof(entry), 1, file->f) != 1) return false;

    *addr = read_u32(entry + 0);
    *size = read_u32(entry + 4);
    return true;
}

bool pe_read_section_header(struct pe_file *file, unsigned num, struct pe_section *out)
{
    if (num > file->sections_num) return false;

    unsigned off = file->sections_off + 0x28 * num;
    if (fseek(file->f, off, SEEK_SET) == -1) return false;

    unsigned char header[0x28];
    if (fread(header, sizeof(header), 1, file->f) != 1) return false;

    memcpy(out->name, header, 8);
    out->name[8] = '\0';

    out->vsize = read_u32(header + 0x08);
    out->address = file->base_addr + read_u32(header + 0x0c);
    out->dsize = read_u32(header + 0x10);
    out->offset = read_u32(header + 0x14);

    return true;
}

bool pe_find_file_offset(struct pe_file *file, unsigned long addr, unsigned long *out)
{
    unsigned char *section_hdrs = malloc(0x28 * file->sections_num);
    if (fseek(file->f, file->sections_off, SEEK_SET) == -1) return false;
    if (fread(section_hdrs, 0x28 * file->sections_num, 1, file->f) != 1) return false;

    for (unsigned long x = 0; x < file->sections_num; x++) {
        unsigned char *header = section_hdrs + 0x28 * x;

        unsigned long address = read_u32(header + 0x0c);
        unsigned long dsize = read_u32(header + 0x10);
        unsigned long offset = read_u32(header + 0x14);

        if (addr >= address && addr < address + dsize) {
            *out = addr - address + offset;
            free(section_hdrs);
            return true;
        }
    }

    free(section_hdrs);
    return false;
}

bool pe_read_name(struct pe_file *file, unsigned long addr, char **out)
{
    unsigned long off;
    if (!pe_find_file_offset(file, addr, &off)) return false;
    if (fseek(file->f, off, SEEK_SET) == -1) return false;

    unsigned str_len = 0;
    char *str = malloc(str_len + 1);
    for (;;) {
        int c = fgetc(file->f);
        if (c == EOF) break;

        str[str_len++] = c;
        str = realloc(str, str_len + 1);
    }
    str[str_len] = '\0';

    *out = str;
    return true;
}

bool pe_read_export_table(struct pe_file *file, struct pe_export *out)
{
    unsigned long off, addr, size;
    if (!pe_get_datadir(file, 0, &addr, &size)) return false;

    // Export table doesn't exist
    if (!addr || !size) {
        memset(out, 0, sizeof(*out));
        return true;
    }

    if (!pe_find_file_offset(file, addr, &off)) return false;
    if (fseek(file->f, off, SEEK_SET) == -1) return false;

    unsigned char header[0x28];
    if (fread(header, sizeof(header), 1, file->f) != 1) return false;

    out->dllname = read_u32(header + 0x0c);
    unsigned long func_num = read_u32(header + 0x14);
    unsigned long name_num = read_u32(header + 0x18);
    unsigned long func_tab_off = read_u32(header + 0x1c) - addr + off;
    unsigned long name_tab_off = read_u32(header + 0x20) - addr + off;
    unsigned long ords_tab_off = read_u32(header + 0x24) - addr + off;

    unsigned char *func_tab = malloc(4 * func_num);
    unsigned char *name_tab = malloc(4 * name_num);
    unsigned char *ords_tab = malloc(4 * name_num);

    if (fseek(file->f, func_tab_off, SEEK_SET) == -1) goto error;
    if (fread(func_tab, 4 * func_num, 1, file->f) != 1) goto error;
    if (fseek(file->f, name_tab_off, SEEK_SET) == -1) goto error;
    if (fread(name_tab, 4 * name_num, 1, file->f) != 1) goto error;
    if (fseek(file->f, ords_tab_off, SEEK_SET) == -1) goto error;
    if (fread(ords_tab, 4 * name_num, 1, file->f) != 1) goto error;

    out->func_num = func_num;
    out->name_num = name_num;
    out->func_off = malloc(sizeof(*out->func_off) * func_num);
    out->name_off = malloc(sizeof(*out->name_off) * func_num);
    memset(out->name_off, 0, sizeof(*out->name_off) * func_num);

    for (unsigned long x = 0; x < func_num; x++) {
        out->func_off[x] = file->base_addr + read_u32(func_tab + 4 * x);
    }

    for (unsigned long x = 0; x < name_num; x++) {
        unsigned ord = read_u16(ords_tab + 2 * x);
        if (ord >= func_num) continue;
        out->name_off[ord] = read_u32(name_tab + 4 * x);
    }

    if (fseek(file->f, name_tab_off, SEEK_SET) == -1) goto error;
    if (fread(name_tab, 4 * name_num, 1, file->f) != 1) goto error;
    free(func_tab);
    free(name_tab);
    return true;

error:
    free(func_tab);
    free(name_tab);
    return false;
}

void pe_free_export_table(struct pe_export *out)
{
    if (out->func_off) free(out->func_off);
    if (out->name_off) free(out->name_off);
}

bool pe_read_import_table(struct pe_file *file, struct pe_import *out)
{
    unsigned long off, addr, size;
    if (!pe_get_datadir(file, 1, &addr, &size)) return false;
    if (!pe_find_file_offset(file, addr, &off)) return false;
    if (fseek(file->f, off, SEEK_SET) == -1) return false;

    unsigned char *header = malloc(size);
    if (fread(header, size, 1, file->f) != 1) {
        free(header);
        return false;
    }

    unsigned long addr_num = 0;
    unsigned long name_num = 0;
    unsigned long *addr_off = malloc(1);
    unsigned long *dllname_off = malloc(1);
    unsigned long *name_off = malloc(1);

    unsigned char *desc = header;
    while (header + size >= desc + 0x14) {
        unsigned long origs_addr = read_u32(desc + 0x0);
        unsigned long dname_addr = read_u32(desc + 0xc);
        unsigned long thunk_addr = read_u32(desc + 0x10);
        if (!origs_addr || !thunk_addr) break;
        desc += 0x14;

        unsigned long origs_off, thunk_off;
        if (!pe_find_file_offset(file, origs_addr, &origs_off)) goto error;
        if (!pe_find_file_offset(file, thunk_addr, &thunk_off)) goto error;

        if (fseek(file->f, origs_off, SEEK_SET) == -1) goto error;
        for (;;) {
            unsigned char info_bin[4];
            if (fread(info_bin, sizeof(info_bin), 1, file->f) != 1) goto error;
            unsigned long info_addr = read_u32(info_bin);
            if (!info_addr) break;

            if (info_addr < 0x80000000) info_addr += 2;

            name_off = realloc(name_off, sizeof(*name_off) * (name_num + 1));
            name_off[name_num++] = info_addr;
        }

        if (fseek(file->f, thunk_off, SEEK_SET) == -1) goto error;
        for (unsigned x = thunk_addr;; x += 4) {
            unsigned char info_bin[4];
            if (fread(info_bin, sizeof(info_bin), 1, file->f) != 1) goto error;
            unsigned long info_addr = read_u32(info_bin);
            if (!info_addr) break;

            addr_off = realloc(addr_off, sizeof(*addr_off) * (addr_num + 1));
            dllname_off = realloc(dllname_off, sizeof(*dllname_off) * (addr_num + 1));
            addr_off[addr_num] = file->base_addr + x;
            dllname_off[addr_num] = dname_addr;
            addr_num++;
        }
    }

    free(header);
    out->addr_num = addr_num;
    out->name_num = name_num;
    out->addr_off = addr_off;
    out->dllname_off = dllname_off;
    out->name_off = name_off;
    return true;

error:
    free(addr_off);
    free(dllname_off);
    free(name_off);
    return false;
}

void pe_free_import_table(struct pe_import *out)
{
    free(out->addr_off);
    free(out->dllname_off);
    free(out->name_off);
}

struct dump_export {
    unsigned num;
    char *dllname;
    char *name;
};

bool dump_find_export(struct pe_file *file, unsigned long addr, struct pe_export *exports, struct dump_export *dump)
{
    for (unsigned x = 0; x < exports->func_num; x++) {
        if (exports->func_off[x] != addr) continue;

        dump->num = x;
        dump->dllname = NULL;
        dump->name = NULL;
        if (!pe_read_name(file, exports->dllname, &dump->dllname)) {
            fprintf(stderr, "Failed to read dllname\n");
            exit(EXIT_FAILURE);
        }
        if (x < exports->name_num) {
            if (!pe_read_name(file, exports->name_off[x], &dump->name)) {
                fprintf(stderr, "Failed to read export name\n");
                exit(EXIT_FAILURE);
            }
        }
        return true;
    }
    return false;
}

unsigned dump_do_export(struct dump_export *dump)
{
    printf(".global _%s_%x\n", dump->dllname, dump->num);
    printf("_%s_%x:\n", dump->dllname, dump->num);
    if (dump->name) {
        char *name = strchr(dump->name, '@');
        if (name) *name = '\0';

        printf(".global %s\n", dump->name);
        printf("%s:\n", dump->name);
        free(dump->name);
    }
    free(dump->dllname);
    return 0;
}

struct dump_import {
    unsigned num;
    char *dllname;
    char *name;
};

bool dump_find_import(struct pe_file *file, unsigned long addr, struct pe_import *imports, struct dump_import *dump)
{
    for (unsigned x = 0; x < imports->addr_num; x++) {
        if (imports->addr_off[x] != addr) continue;

        dump->num = 0;
        dump->dllname = NULL;
        dump->name = NULL;
        if (!pe_read_name(file, imports->dllname_off[x], &dump->dllname)) {
            fprintf(stderr, "Failed to read dllname\n");
            exit(EXIT_FAILURE);
        }
        if (x < imports->name_num) {
            unsigned off = imports->name_off[x];
            if (off >= 0x80000000) {
                dump->num = off - 0x80000000;
            } else if (!pe_read_name(file, imports->name_off[x], &dump->name)) {
                fprintf(stderr, "Failed to read import name\n");
                exit(EXIT_FAILURE);
            }
        }
        return true;
    }
    return false;
}

unsigned dump_do_import(struct dump_import *dump, char *binfile)
{
    (void)binfile;
    unsigned size = 0;

    if (dump->name) {
        char *name = strchr(dump->name, '@');
        if (name) *name = '\0';

        printf(".long %s # %s\n", dump->name, dump->dllname);
        free(dump->name);
        size = 4;
    } else {
        printf(".long _%s_%x\n", dump->dllname, dump->num);
        size = 4;
    }
    free(dump->dllname);
    return size;
}

int dump_asm(struct pe_file *file, char *binfile)
{
    struct pe_export exports;
    if (!pe_read_export_table(file, &exports)) {
        fprintf(stderr, "Failed to read export table\n");
        return EXIT_FAILURE;
    }

    struct pe_import imports;
    if (!pe_read_import_table(file, &imports)) {
        fprintf(stderr, "Failed to read import table\n");
        return EXIT_FAILURE;
    }

    printf(".global pe_start\n");
    printf("pe_start = 0x%lx\n", pe_get_entrypoint(file));

    for (unsigned x = 0; x < pe_get_sections_num(file); x++) {
        struct pe_section sec;
        if (!pe_read_section_header(file, x, &sec)) {
            fprintf(stderr, "Failed to read section %d\n", x);
            return EXIT_FAILURE;
        }

        char *name = sec.name;
        if (*name == '.') name++;

        printf("\n");
        printf("pe_%s_addr = 0x%lx\n", name, sec.address);
        printf(".global pe_%s_addr\n", name);
        printf(".section .pe_%s, \"awx\"\n", name);

        unsigned long size = sec.dsize;
        if (sec.vsize > sec.dsize) size = sec.vsize;

        unsigned long pos = 0;
        unsigned long last_pos = 0;
        while (pos <= size) {
            bool dump = false;

            struct dump_export dump_export;
            bool found_export = dump_find_export(file, sec.address + pos, &exports, &dump_export);
            if (found_export) dump = true;

            struct dump_import dump_import;
            bool found_import = dump_find_import(file, sec.address + pos, &imports, &dump_import);
            if (found_import) dump = true;

            if (pos >= size) {
                dump = true;
            }

            if (!dump) {
                pos++;
                continue;
            }

            if (last_pos == pos) {
                // nothing!
            } else if (pos <= sec.dsize) {
                printf(".incbin \"%s\", 0x%lx, 0x%lx\n", binfile, sec.offset + last_pos, pos - last_pos);
            } else if (last_pos < sec.dsize) {
                printf(".incbin \"%s\", 0x%lx, 0x%lx\n", binfile, sec.offset + last_pos, sec.dsize - last_pos);
                printf(".zero 0x%lx\n", pos - sec.dsize);
            } else {
                printf(".zero 0x%lx\n", pos - last_pos);
            }
            last_pos = pos;

            if (found_export) pos += dump_do_export(&dump_export);
            if (found_import) pos += dump_do_import(&dump_import, binfile);

            if (last_pos != pos) {
                last_pos = pos;
            } else {
                pos++;
            }
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct pe_file *file = pe_open(argv[1]);
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return EXIT_FAILURE;
    }

    char *binfile = strrchr(argv[1], '/');
    if (!binfile) binfile = argv[1];
    if (*binfile == '/') binfile++;

    return dump_asm(file, binfile);
}
