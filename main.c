#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct {
    char *out;
    char *incbin;
    bool win;
} opt;

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
    bool r, w, x;
};

struct pe_export {
    unsigned long dllname;
    unsigned long func_num;
    unsigned long name_num;
    unsigned long *func_addr;
    unsigned long *name_addr;
    unsigned long *ords;
};

struct pe_import {
    unsigned long addr_num;
    unsigned long name_num;
    unsigned long *imp_addr;
    unsigned long *dllname_addr;
    unsigned long *name_addr;
};

struct pe_rsrc_string {
    unsigned long num;
    unsigned long *id;
    unsigned long *addr;
    unsigned long *size;
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
    unsigned long flags = read_u32(header + 0x24);
    out->x = flags & 0x20000000;
    out->r = flags & 0x40000000;
    out->w = flags & 0x80000000;

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
    out->func_addr = malloc(sizeof(*out->func_addr) * func_num);
    out->name_addr = malloc(sizeof(*out->name_addr) * func_num);
    memset(out->name_addr, 0, sizeof(*out->name_addr) * func_num);

    for (unsigned long x = 0; x < func_num; x++) {
        out->func_addr[x] = file->base_addr + read_u32(func_tab + 4 * x);
    }

    for (unsigned long x = 0; x < name_num; x++) {
        unsigned ord = read_u16(ords_tab + 2 * x);
        if (ord >= func_num) continue;
        out->name_addr[ord] = read_u32(name_tab + 4 * x);
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
    if (out->func_addr) free(out->func_addr);
    if (out->name_addr) free(out->name_addr);
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
    unsigned long *imp_addr = NULL;
    unsigned long *dllname_addr = NULL;
    unsigned long *name_addr = NULL;

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

            name_addr = realloc(name_addr, sizeof(*name_addr) * (name_num + 1));
            name_addr[name_num++] = info_addr;
        }

        if (fseek(file->f, thunk_off, SEEK_SET) == -1) goto error;
        for (unsigned x = thunk_addr;; x += 4) {
            unsigned char info_bin[4];
            if (fread(info_bin, sizeof(info_bin), 1, file->f) != 1) goto error;
            unsigned long info_addr = read_u32(info_bin);
            if (!info_addr) break;

            imp_addr = realloc(imp_addr, sizeof(*imp_addr) * (addr_num + 1));
            dllname_addr = realloc(dllname_addr, sizeof(*dllname_addr) * (addr_num + 1));
            imp_addr[addr_num] = file->base_addr + x;
            dllname_addr[addr_num] = dname_addr;
            addr_num++;
        }
    }

    free(header);
    out->addr_num = addr_num;
    out->name_num = name_num;
    out->imp_addr = imp_addr;
    out->dllname_addr = dllname_addr;
    out->name_addr = name_addr;
    return true;

error:
    free(imp_addr);
    free(dllname_addr);
    free(name_addr);
    return false;
}

void pe_free_import_table(struct pe_import *out)
{
    free(out->imp_addr);
    free(out->dllname_addr);
    free(out->name_addr);
}

struct pe_dir_header {
    unsigned num_name;
    unsigned num_id;
};

bool pe_dir_read_header(struct pe_file *file, struct pe_dir_header *out)
{
    unsigned char header[0x10];
    if (fread(header, sizeof(header), 1, file->f) != 1) return false;

    out->num_name = read_u16(header + 0xc);
    out->num_id = read_u16(header + 0xe);

    return true;
}

struct pe_dir_entry {
    unsigned long id;
    unsigned long off;
    bool dir;
};

bool pe_dir_read_entry(struct pe_file *file, struct pe_dir_entry *out)
{
    unsigned char entry[0x8];
    if (fread(entry, sizeof(entry), 1, file->f) != 1) return false;

    out->id = read_u32(entry + 0);
    unsigned long off = read_u32(entry + 4);
    out->off = off & 0x7fffffff;
    out->dir = off & 0x80000000;

    return true;
}

bool pe_get_rsrc_dir(struct pe_file *file, unsigned type, unsigned long *off, unsigned long *dir_off, unsigned long *dir_addr)
{
    unsigned long dir_size;
    if (!pe_get_datadir(file, 2, dir_addr, &dir_size)) return false;

    // Resoruces doesn't exist
    if (!*dir_addr || !dir_size) {
        *off = 0;
        return true;
    }

    if (!pe_find_file_offset(file, *dir_addr, dir_off)) return false;
    if (fseek(file->f, *dir_off, SEEK_SET) == -1) return false;

    // Read header and skip named entries
    struct pe_dir_header header;
    if (!pe_dir_read_header(file, &header)) return false;
    if (fseek(file->f, header.num_name * 8, SEEK_CUR) == -1) return false;

    // Parse ID entries
    while (header.num_id--) {
        struct pe_dir_entry entry;
        if (!pe_dir_read_entry(file, &entry)) return false;

        if (entry.id != type) continue;

        // Make sure we're actually getting a dir
        if (!entry.dir) return false;

        *off = *dir_off + entry.off;
        return true;
    }

    // Didn't find the entry
    *off = 0;
    return true;
}

bool pe_read_rsrc_strings(struct pe_file *file, struct pe_rsrc_string *out)
{
    unsigned long off, dir_off, dir_addr;
    if (!pe_get_rsrc_dir(file, 6, &off, &dir_off, &dir_addr)) return false;

    if (!off) {
        memset(out, 0, sizeof(*out));
        return true;
    }

    if (fseek(file->f, off, SEEK_SET) == -1) return false;

    // Read header
    struct pe_dir_header header;
    if (!pe_dir_read_header(file, &header)) return false;

    unsigned long *rsrc_id = malloc(sizeof(unsigned long) * header.num_id);
    unsigned long *rsrc_addr = malloc(sizeof(unsigned long) * header.num_id);
    unsigned long *rsrc_size = malloc(sizeof(unsigned long) * header.num_id);

    unsigned long entry_off_base = off + 0x10 + header.num_name * 8;
    for (unsigned x = 0; x < header.num_id; x++) {
        unsigned long entry_off = entry_off_base + x * 8;

        struct pe_dir_entry entry;
        if (fseek(file->f, entry_off, SEEK_SET) == -1) goto error;
        if (!pe_dir_read_entry(file, &entry)) goto error;

        if (!entry.dir) goto error;
        unsigned long sub_off = dir_off + entry.off;
        struct pe_dir_header sub_header;
        if (fseek(file->f, sub_off, SEEK_SET) == -1) goto error;
        if (!pe_dir_read_header(file, &sub_header)) goto error;

        // These are the expected values for metrowerks binaries...
        // It's not entirely clear what the expected constraints are.
        if (sub_header.num_name != 0) goto error;
        if (sub_header.num_id != 1) goto error;

        struct pe_dir_entry sub_entry;
        if (!pe_dir_read_entry(file, &sub_entry)) goto error;

        // Again, checking for expected values
        if (sub_entry.dir) goto error;
        if (sub_entry.id != 0x409) goto error;
        if (fseek(file->f, dir_off + sub_entry.off, SEEK_SET) == -1) goto error;

        unsigned char data_header[0x10];
        if (fread(data_header, sizeof(data_header), 1, file->f) != 1) goto error;

        unsigned long str_addr = file->base_addr + read_u32(data_header + 0);
        unsigned long str_size = read_u32(data_header + 4);

        rsrc_id[x] = entry.id;
        rsrc_addr[x] = str_addr;
        rsrc_size[x] = str_size;
    }

    out->num = header.num_id;
    out->id = rsrc_id;
    out->addr = rsrc_addr;
    out->size = rsrc_size;
    return true;

error:
    free(rsrc_id);
    free(rsrc_addr);
    return false;
}

void pe_free_rsrc_strings(struct pe_rsrc_string *out)
{
    free(out->id);
    free(out->addr);
    free(out->size);
}

struct dump_export {
    unsigned num;
    char *dllname;
    char *name;
};

bool dump_find_export(struct pe_file *file, unsigned long addr, struct pe_export *exports, struct dump_export *dump)
{
    for (unsigned x = 0; x < exports->func_num; x++) {
        if (exports->func_addr[x] != addr) continue;

        dump->num = x;
        dump->dllname = NULL;
        dump->name = NULL;
        if (!pe_read_name(file, exports->dllname, &dump->dllname)) {
            fprintf(stderr, "Failed to read dllname\n");
            exit(EXIT_FAILURE);
        }
        if (x < exports->name_num) {
            if (!pe_read_name(file, exports->name_addr[x], &dump->name)) {
                fprintf(stderr, "Failed to read export name\n");
                exit(EXIT_FAILURE);
            }
        }
        return true;
    }
    return false;
}

unsigned dump_do_export(FILE *out, struct dump_export *dump, const char *incbin, unsigned long pos)
{
    (void)incbin;
    (void)pos;

    fprintf(out, ".global _%s_%x\n", dump->dllname, dump->num);
    fprintf(out, "_%s_%x:\n", dump->dllname, dump->num);
    if (dump->name) {
        char *name = strchr(dump->name, '@');
        if (name) *name = '\0';

        fprintf(out, ".global %s\n", dump->name);
        fprintf(out, "%s:\n", dump->name);
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
        if (imports->imp_addr[x] != addr) continue;

        dump->num = 0;
        dump->dllname = NULL;
        dump->name = NULL;
        if (!pe_read_name(file, imports->dllname_addr[x], &dump->dllname)) {
            fprintf(stderr, "Failed to read dllname\n");
            exit(EXIT_FAILURE);
        }
        if (x < imports->name_num) {
            unsigned off = imports->name_addr[x];
            if (off >= 0x80000000) {
                dump->num = off - 0x80000000;
            } else if (!pe_read_name(file, imports->name_addr[x], &dump->name)) {
                fprintf(stderr, "Failed to read import name\n");
                exit(EXIT_FAILURE);
            }
        }
        return true;
    }
    return false;
}

unsigned dump_do_import(FILE *out, struct dump_import *dump, const char *incbin, unsigned long pos)
{
    (void)incbin;
    (void)pos;
    unsigned size = 0;

    if (dump->name) {
        char *name = strchr(dump->name, '@');
        if (name) *name = '\0';

        fprintf(out, ".long %s%s # %s\n", opt.win ? "_" : "",
            dump->name, dump->dllname);
        free(dump->name);
        size = 4;
    } else {
        fprintf(out, ".long _%s_%x\n", dump->dllname, dump->num);
        size = 4;
    }
    free(dump->dllname);
    return size;
}

struct dump_rsrc_string {
    unsigned long id;
    unsigned long size;
};

bool dump_find_rsrc_string(struct pe_file *file, unsigned long addr, struct pe_rsrc_string *rsrc_strings, struct dump_rsrc_string *dump)
{
    (void)file;

    for (unsigned x = 0; x < rsrc_strings->num; x++) {
        if (rsrc_strings->addr[x] != addr) continue;

        dump->id = rsrc_strings->id[x];
        dump->size = rsrc_strings->size[x];
        return true;
    }

    return false;
}

unsigned dump_do_rsrc_string(FILE *out, struct dump_rsrc_string *dump, const char *incbin, unsigned long pos)
{
    (void)incbin;
    (void)pos;

    fprintf(out, ".global pe_rsrc_strings_%ld\n"
        "pe_rsrc_strings_%ld:\n"
        "    .incbin \"%s\", 0x%lx, 0x%lx\n",
        dump->id, dump->id, incbin, pos, dump->size);
    return dump->size;
}

int dump_asm(FILE *out, struct pe_file *file, const char *incbin)
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

    struct pe_rsrc_string rsrc_strings;
    if (!pe_read_rsrc_strings(file, &rsrc_strings)) {
        fprintf(stderr, "Failed to read string resources\n");
        return EXIT_FAILURE;
    }

    fprintf(out, ".global pe_start\n");
    fprintf(out, "pe_start = 0x%lx\n", pe_get_entrypoint(file));

    for (unsigned x = 0; x < pe_get_sections_num(file); x++) {
        struct pe_section sec;
        if (!pe_read_section_header(file, x, &sec)) {
            fprintf(stderr, "Failed to read section %d\n", x);
            return EXIT_FAILURE;
        }

        bool bss = strcmp(sec.name, ".bss") == 0;

        char *name = sec.name;
        if (*name == '.') name++;

        fprintf(out, "\n"
            ".global pe_%s_addr\n"
            "pe_%s_addr = 0x%lx\n"
            ".section .pe_%s, \"%s%s%s%s\"%s\n",
            name,
            name, sec.address,
            name,
            sec.r ? "a" : "", sec.w ? "w" : (opt.win ? "r" : ""),
            sec.x ? "x" : "",
            (opt.win && bss) ? "b" : "",
            (!opt.win && bss) ? ", @nobits" : ""
        );

        unsigned long size = sec.vsize;

        unsigned long pos = 0;
        unsigned long last_pos = 0;
        while (pos <= size) {
            bool dump = false;

            struct dump_export dump_export;
            bool found_export =
                dump_find_export(file, sec.address + pos,
                &exports, &dump_export);
            if (found_export) dump = true;

            struct dump_import dump_import;
            bool found_import =
                dump_find_import(file, sec.address + pos,
                &imports, &dump_import);
            if (found_import) dump = true;

            struct dump_rsrc_string dump_rsrc_string;
            bool found_rsrc_string =
                dump_find_rsrc_string(file, sec.address + pos,
                &rsrc_strings, &dump_rsrc_string);
            if (found_rsrc_string) dump = true;

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
                fprintf(out, ".incbin \"%s\", 0x%lx, 0x%lx\n", incbin,
                    sec.offset + last_pos, pos - last_pos);
            } else if (last_pos < sec.dsize) {
                fprintf(out, ".incbin \"%s\", 0x%lx, 0x%lx\n", incbin,
                    sec.offset + last_pos, sec.dsize - last_pos);
                fprintf(out, ".zero 0x%lx\n", pos - sec.dsize);
            } else {
                fprintf(out, ".zero 0x%lx\n", pos - last_pos);
            }
            last_pos = pos;

            if (found_export) {
                pos += dump_do_export(out, &dump_export, incbin,
                    sec.offset + last_pos);
            }
            if (found_import) {
                pos += dump_do_import(out, &dump_import, incbin,
                    sec.offset + last_pos);
            }
            if (found_rsrc_string) {
                pos += dump_do_rsrc_string(out, &dump_rsrc_string, incbin,
                    sec.offset + last_pos);
            }

            if (last_pos != pos) {
                last_pos = pos;
            } else {
                pos++;
            }
        }
    }

    fprintf(out, "\n"
        ".section .pe_rsrc_strings, \"a\"\n"
        ".global pe_rsrc_strings\n"
        "pe_rsrc_strings:\n"
    );
    for (unsigned x = 0; x < rsrc_strings.num; x++) {
        unsigned long id = rsrc_strings.id[x];
        fprintf(out, "    .long %ld, pe_rsrc_strings_%ld\n", id, id);
    }
    fprintf(out, "    .long 0\n");

    pe_free_export_table(&exports);
    pe_free_import_table(&imports);
    pe_free_rsrc_strings(&rsrc_strings);
    return EXIT_SUCCESS;
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
        } else if (strcmp(argv[1], "--win") == 0) {
            opt.win = true;
        }
        argv++; argc--;
    }

    if (argc <= 1) {
        main_usage();
        return EXIT_FAILURE;
    }

    struct pe_file *file = pe_open(argv[1]);
    if (!file) {
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

    return dump_asm(out, file, incbin);
}
