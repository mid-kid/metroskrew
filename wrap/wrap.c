#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#else
#include <spawn.h>
#include <sys/wait.h>
extern char **environ;
#endif

#define PROGRAM_NAME "skrewrap"

#define DEFAULT_MWCCARM "4.0-1051"
#define DEFAULT_MWLDARM "2.0-99"
#define DEFAULT_MWASMARM "1.0-27"

#define DEFAULT_CFG_FILE ".mwconfig"
#define VER_CFG 1

#ifdef _UNICODE
#define FMT_TS "%ls"
#else
#define FMT_TS "%s"
#endif

// POSIX-compatibility
#ifndef _WIN32
typedef char _TCHAR;
#define _tmain main
#define _T(...) __VA_ARGS__
#define _ftprintf(...) fprintf(__VA_ARGS__)
#define _sntprintf(...) snprintf(__VA_ARGS__)
#define _tcscat(...) strcat(__VA_ARGS__)
#define _tcschr(...) strchr(__VA_ARGS__)
#define _tcscmp(...) strcmp(__VA_ARGS__)
#define _tcscspn(...) strcspn(__VA_ARGS__)
#define _tcsdup(...) strdup(__VA_ARGS__)
#define _tcslen(...) strlen(__VA_ARGS__)
#define _tcsrchr(...) strrchr(__VA_ARGS__)
#define _tfopen(...) fopen(__VA_ARGS__)
#define _tgetenv(...) getenv(__VA_ARGS__)
#endif

#ifndef _WIN32
#define PATH_DELIM "/"
#else
#define PATH_DELIM "/\\"
#endif

struct args {
    _TCHAR *o;
    _TCHAR *precompile;
    bool M;
    bool MD;

    bool wrap_dbg;
    _TCHAR *wrap_ver;
    _TCHAR *wrap_sdk;
};

struct file {
    size_t size;
    unsigned char data[];
};

struct args parse_args(int argc, _TCHAR *argv[], int *out_argc, _TCHAR ***out_argv)
{
    struct args args = {
        .o = NULL,
        .precompile = NULL,
        .M = false,
        .MD = false,
        .wrap_dbg = false,
        .wrap_ver = NULL,
        .wrap_sdk = NULL
    };

    int new_argc = 0;
    _TCHAR **new_argv = malloc(sizeof(_TCHAR *) * (argc + 2));
    new_argv[new_argc++] = NULL;

    while (argc >= 1) {
        int skip = 0;
        int copy = 0;

        if (_tcscmp(argv[0], _T("-o")) == 0 &&
                argc >= 2) {
            args.o = argv[1];
            copy = 2;
        } else if (_tcscmp(argv[0], _T("-precompile")) == 0 &&
                argc >= 2) {
            args.precompile = argv[1];
            copy = 2;
        } else if (_tcscmp(argv[0], _T("-M")) == 0) {
            args.M = true;
            copy = 1;
        } else if (_tcscmp(argv[0], _T("-MD")) == 0) {
            args.MD = true;
            copy = 1;
        } else if (_tcscmp(argv[0], _T("-wrap:dbg")) == 0) {
            args.wrap_dbg = true;
            skip = 1;
        } else if (_tcscmp(argv[0], _T("-wrap:ver")) == 0 && argc >= 2) {
            args.wrap_ver = argv[1];
            skip = 2;
        } else if (_tcscmp(argv[0], _T("-wrap:sdk")) == 0 && argc >= 2) {
            args.wrap_sdk = argv[1];
            skip = 2;
        } else {
            copy = 1;
        }

        argc -= skip + copy;
        while (skip--) argv++;
        while (copy--) new_argv[new_argc++] = *argv++;
    }
    new_argv[new_argc] = NULL;

    *out_argc = new_argc;
    *out_argv = new_argv;
    return args;
}

struct file *file_read(const _TCHAR *name)
{
    FILE *f = _tfopen(name, _T("rb"));
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

char *tctoutf(const _TCHAR *str)
{
    if (!str) return NULL;
#ifdef _UNICODE
    size_t size = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (!size) return NULL;
    char *res = malloc(size);
    if (!res) return NULL;
    WideCharToMultiByte(CP_UTF8, 0, str, -1, res, size, NULL, NULL);
    return res;
#else
    return strdup(str);
#endif
}

_TCHAR *utftotc(const char *str)
{
    if (!str) return NULL;
#ifdef _UNICODE
    size_t size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (!size) return NULL;
    _TCHAR *res = malloc(size);
    if (!res) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, str, -1, res, size);
    return res;
#else
    return strdup(str);
#endif
}

struct config {
    char *wine;
    char *path_unx;
    char *path_win;
    char *path_build_unx;
    char *path_build_win;
};

const _TCHAR *cfg_file(void)
{
    const _TCHAR *path = _tgetenv(_T("MWCONFIG"));
    if (!path) path = _T(DEFAULT_CFG_FILE);
    return path;
}

void cfg_save_writestr(FILE *f, const char *str)
{
    if (str) fputs(str, f);
    fputc('\0', f);
}

void cfg_save(struct config cfg)
{
    FILE *f = _tfopen(cfg_file(), _T("wb"));
    if (!f) {
        fprintf(stderr, PROGRAM_NAME ": cfg_save: Failed to write config\n");
        exit(EXIT_FAILURE);
    }

    const char head[] = {'M', 'W', 'R', VER_CFG};
    fwrite(head, sizeof(head), 1, f);
    cfg_save_writestr(f, cfg.wine);
    cfg_save_writestr(f, cfg.path_unx);
    cfg_save_writestr(f, cfg.path_win);
    cfg_save_writestr(f, cfg.path_build_unx);
    cfg_save_writestr(f, cfg.path_build_win);
    fclose(f);
}

char *cfg_load_readstr(struct file *file, size_t *pos)
{
    // Check if a string is present
    if (file->size < *pos + 1) return NULL;
    if (file->data[*pos] == '\0') {
        // Treat empty string as empty
        (*pos)++;
        return NULL;
    }

    // Look for the null byte
    size_t readlen = strnlen((char *)file->data + *pos, file->size - *pos);
    if (file->size - *pos <= readlen) return NULL;

    // If found, we can safely read it
    char *readstr = strdup((char *)file->data + *pos);
    *pos += readlen + 1;
    return readstr;
}

struct config cfg_load(void)
{
    struct config cfg;
    cfg.wine = NULL;
    cfg.path_unx = NULL;
    cfg.path_win = NULL;
    cfg.path_build_unx = NULL;
    cfg.path_build_win = NULL;

    struct file *file = file_read(cfg_file());
    if (!file) return cfg;
    size_t file_pos = 0;

    const char head[] = {'M', 'W', 'R', VER_CFG};
    if (file->size < file_pos + sizeof(head)) return cfg;
    if (memcmp(file->data + file_pos, head, sizeof(head)) != 0) return cfg;
    file_pos += sizeof(head);

    cfg.wine = cfg_load_readstr(file, &file_pos);
    cfg.path_unx = cfg_load_readstr(file, &file_pos);
    cfg.path_win = cfg_load_readstr(file, &file_pos);
    cfg.path_build_unx = cfg_load_readstr(file, &file_pos);
    cfg.path_build_win = cfg_load_readstr(file, &file_pos);

    free(file);
    return cfg;
}

void cfg_free(struct config cfg)
{
    free(cfg.path_win);
    free(cfg.path_unx);
    free(cfg.path_build_win);
    free(cfg.path_build_unx);
    free(cfg.wine);
}

void configure(int argc, _TCHAR *argv[])
{
    _TCHAR *wine = NULL;
    _TCHAR *path_unx = NULL;
    _TCHAR *path_win = NULL;
    _TCHAR *path_build_unx = NULL;
    _TCHAR *path_build_win = NULL;

    while (argc >= 1) {
        if (_tcscmp(argv[0], _T("-wine")) == 0) {
            wine = argv[1];
            argv += 2; argc -= 2;
        } else if (_tcscmp(argv[0], _T("-path_unx")) == 0) {
            path_unx = argv[1];
            argv += 2; argc -= 2;
        } else if (_tcscmp(argv[0], _T("-path_win")) == 0) {
            path_win = argv[1];
            argv += 2; argc -= 2;
        } else if (_tcscmp(argv[0], _T("-path_build_unx")) == 0) {
            path_build_unx = argv[1];
            argv += 2; argc -= 2;
        } else if (_tcscmp(argv[0], _T("-path_build_win")) == 0) {
            path_build_win = argv[1];
            argv += 2; argc -= 2;
        } else {
            fprintf(stderr,
                PROGRAM_NAME ": configure: Invalid argument: " FMT_TS "\n",
                *argv);
            exit(EXIT_FAILURE);
        }
    }

    struct config cfg;
    cfg.wine = tctoutf(wine);
    cfg.path_unx = tctoutf(path_unx);
    cfg.path_win = tctoutf(path_win);
    cfg.path_build_unx = tctoutf(path_build_unx);
    cfg.path_build_win = tctoutf(path_build_win);
    if ((wine && !cfg.wine) ||
            (path_unx && !cfg.path_unx) ||
            (path_win && !cfg.path_win) ||
            (path_build_unx && !cfg.path_build_unx) ||
            (path_build_win && !cfg.path_build_win)) {
        fprintf(stderr, PROGRAM_NAME ": configure: Conversion failed\n");
        exit(EXIT_FAILURE);
    }
    cfg_save(cfg);
    cfg_free(cfg);
}

_TCHAR *my_dirname(_TCHAR *str)
{
    // Returns an empty string if no slash is found

    size_t sep = _tcslen(str);
    while (sep > 0) if (_tcschr(_T(PATH_DELIM), str[--sep])) break;
    str = realloc(str, sizeof(*str) * (sep + 1));
    str[sep] = '\0';
    return str;
}

void str_resize(_TCHAR **str, size_t *max, size_t req)
{
    if (*max < req) {
        while (*max < req) *max += 1024;
        *str = realloc(*str, sizeof(**str) * *max);
    }
}

// Windows argv[] strings are passed as a whole, instead of being split up into
// an array. This means that *we* have to make sure to quote our arguments, and
// for some reason nobody ever thought to make a library for this.
_TCHAR *win_argv_build(const _TCHAR *const *argv)
{
    size_t args_max = 1024;
    size_t args_pos = 0;
    _TCHAR *args = malloc(sizeof(_TCHAR) * args_max);

    bool first = true;

    while (*argv) {
        const _TCHAR *arg = *argv;

        // Add a space between arguments
        if (!first) {
            str_resize(&args, &args_max, args_pos + 1);
            args[args_pos++] = ' ';
        }
        first = false;

        // If it doesn't contain any special chars, copy as-is
        size_t arg_len = _tcslen(arg);
        if (_tcscspn(arg, _T(" \t\n\r")) == arg_len) {
            str_resize(&args, &args_max, args_pos + arg_len);
            memcpy(args + args_pos, arg, sizeof(_TCHAR) * arg_len);
            args_pos += arg_len;
            argv++;
            continue;
        }

        // Quote the argument
        int backslashes = 0;
        str_resize(&args, &args_max, args_pos + 1);
        args[args_pos++] = '"';
        while (*arg) {
            if (*arg == '\\') {
                backslashes++;
            } else if (*arg == '"') {
                // Add extra backslashes to escape the quote
                backslashes++;
                str_resize(&args, &args_max, args_pos + backslashes);
                while (backslashes--) args[args_pos++] = '\\';
                backslashes = 0;
            } else {
                backslashes = 0;
            }

            str_resize(&args, &args_max, args_pos + 1);
            args[args_pos++] = *arg;

            arg++;
        }
        // Add extra backslashes to avoid escaping the final quote
        if (backslashes) {
            str_resize(&args, &args_max, args_pos + backslashes);
            while (backslashes--) args[args_pos++] = '\\';
        }
        str_resize(&args, &args_max, args_pos + 1);
        args[args_pos++] = '"';

        argv++;
    }

    args = realloc(args, sizeof(*args) * (args_pos + 1));
    args[args_pos] = '\0';

    return args;
}

void fix_depfile(_TCHAR *fname, const char *path_unx, const char *path_win, const char *path_build_unx, const char *path_build_win)
{
    struct file *file = file_read(fname);
    if (!file) return;

    FILE *f = _tfopen(fname, _T("wb"));
    if (!f) {
        free(file);
        return;
    }

    size_t size_win = 0;
    if (path_win && path_unx) {
        size_win = strlen(path_win);
    }
    size_t size_build_win = 0;
    if (path_build_win && path_build_unx) {
        size_build_win = strlen(path_build_win);
    }

    bool blankline = false;

    // Replace any instances of path_win at the beginning of a line with
    // path_unx, and backslashes with forward slashes.
    for (size_t x = 0; x < file->size;) {
        if (size_win && blankline &&
                file->size - x > size_win &&
                memcmp(file->data + x, path_win, size_win) == 0) {
            fputs(path_unx, f);
            x += size_win;
        } else if (size_build_win && blankline &&
                file->size - x > size_build_win &&
                memcmp(file->data + x, path_build_win, size_build_win) == 0) {
            fputs(path_build_unx, f);
            x += size_build_win;
        } else if (file->size - x > 2 && file->data[x] == '\\' &&
                file->data[x + 1] != '\r' && file->data[x + 1] != '\n') {
            fputc('/', f);
            x++;
        } else {
            if (blankline && file->data[x] != ' ' && file->data[x] != '\t') {
                blankline = false;
            }
            if (file->data[x] == '\n') blankline = true;
            fputc(file->data[x++], f);
        }
    }

    fclose(f);
    free(file);
}

int _tmain(int argc, _TCHAR *argv[])
{
    if (argc < 2) {
        fprintf(stderr, PROGRAM_NAME ": Too few arguments\n");
        return EXIT_FAILURE;
    }

    if (_tcscmp(argv[1], _T("-conf")) == 0) {
        configure(argc - 2, argv + 2);
        return EXIT_SUCCESS;
    }

    struct config cfg = cfg_load();

    // Filter the arguments to pass to the application
    int new_argc;
    _TCHAR **new_argv;
    struct args args = parse_args(argc - 2, argv + 2, &new_argc, &new_argv);

    _TCHAR *tool_dir = _tcsdup(argv[0]);
    tool_dir = my_dirname(tool_dir);
    if (!*tool_dir) {
#ifndef _WIN32
        size_t tool_dir_len = 0;
        ssize_t res;
        do {
            free(tool_dir);
            tool_dir_len += 0x1000;
            tool_dir = malloc(sizeof(_TCHAR) * tool_dir_len);
        } while ((res = readlink("/proc/self/exe", tool_dir, tool_dir_len))
            == tool_dir_len);
        if (res == -1) {
            perror(PROGRAM_NAME ": readlink");
            exit(EXIT_FAILURE);
        }
        tool_dir = my_dirname(tool_dir);
#endif
    }

    const _TCHAR *tool_bin = argv[1];
    const _TCHAR *tool_ver = NULL;
    const _TCHAR *tool_sdk = NULL;

    if (args.wrap_ver) tool_ver = args.wrap_ver;
    if (args.wrap_sdk) tool_sdk = args.wrap_sdk;

    // If no version was specified, pick a default for generic binary names
    if (!tool_ver && !tool_sdk) {
        if (_tcscmp(tool_bin, _T("mwccarm")) == 0) {
            tool_ver = _T(DEFAULT_MWCCARM);
        }
        if (_tcscmp(tool_bin, _T("mwldarm")) == 0) {
            tool_ver = _T(DEFAULT_MWLDARM);
        }
        if (_tcscmp(tool_bin, _T("mwasmarm")) == 0) {
            tool_ver = _T(DEFAULT_MWASMARM);
        }
    }

    // Make a path of the chosen tool
    _TCHAR *tool = NULL;
    if (tool_sdk) {
        // Not implemented
        return EXIT_FAILURE;
    } else if (tool_ver) {
        size_t tool_size = (*tool_dir ? _tcslen(tool_dir) + 1 : 0) +
            _tcslen(tool_bin) + 1 + _tcslen(tool_ver) + 5;
        tool = malloc(sizeof(_TCHAR) * tool_size);
        _sntprintf(tool, tool_size, _T(FMT_TS FMT_TS FMT_TS "-" FMT_TS ".exe"),
            tool_dir, *tool_dir ? "/" : "", tool_bin, tool_ver);
    } else {
        size_t tool_size = (*tool_dir ? _tcslen(tool_dir) + 1 : 0) +
            _tcslen(tool_bin) + 5;
        tool = malloc(sizeof(_TCHAR) * tool_size);
        _sntprintf(tool, tool_size, _T(FMT_TS FMT_TS FMT_TS ".exe"),
            tool_dir, *tool_dir ? "/" : "", tool_bin);
    }
    new_argv[0] = tool;

    // Add the wine command if requested
    _TCHAR *wine = utftotc(cfg.wine);
    if (wine) {
        new_argc += 1;
        new_argv = realloc(new_argv, sizeof(*new_argv) * (new_argc + 1));
        memmove(new_argv + 1, new_argv, sizeof(*new_argv) * new_argc);
        new_argv[0] = wine;
    }

    // Build standard library paths for environment variables
    size_t mwcincludes_size = _tcslen(tool_dir) * 3 + 46;
    _TCHAR *MWCIncludes = malloc(sizeof(_TCHAR) * mwcincludes_size);
    _sntprintf(MWCIncludes, mwcincludes_size, _T(
        FMT_TS "/" FMT_TS ";"
        FMT_TS "/" FMT_TS ";"
        FMT_TS "/" FMT_TS),
        tool_dir, _T("include"),
        tool_dir, _T("include/MSL_C"),
        tool_dir, _T("include/MSL_Extras"));

    size_t mwclibraries_size = _tcslen(tool_dir) + 5;
    _TCHAR *MWLibraries = malloc(sizeof(_TCHAR) * mwclibraries_size);
    _sntprintf(MWLibraries, mwclibraries_size, _T(
        FMT_TS "/" FMT_TS),
        tool_dir, _T("lib"));

    size_t mwclibraryfiles_size = 107;
    _TCHAR *MWLibraryFiles = malloc(sizeof(_TCHAR) * mwclibraryfiles_size);
    _sntprintf(MWLibraryFiles, mwclibraryfiles_size, _T(
        "MSL_C_NITRO_Ai_LE.a" ";"
        "MSL_Extras_NITRO_Ai_LE.a" ";"
        "MSL_CPP_NITRO_Ai_LE.a" ";"
        "FP_fastI_v5t_LE.a" ";"
        "NITRO_Runtime_Ai_LE.a"));

#ifndef _WIN32
    // Set up the environment
    setenv("MWCIncludes", MWCIncludes, true);
    setenv("MWLibraries", MWLibraries, true);
    setenv("MWLibraryFiles", MWLibraryFiles, true);

    // Execute the tool
    if (args.wrap_dbg) {
        for (int x = 0; x < new_argc; x++) {
            printf(x ? " %s" : "%s", new_argv[x]);
        }
        printf("\n");
    }
    pid_t pid;
    if (posix_spawnp(&pid, new_argv[0], NULL, NULL, new_argv, environ) != 0) {
        perror(PROGRAM_NAME ": posix_spawnp");
        exit(EXIT_FAILURE);
    }
    int exitcode;
    waitpid(pid, &exitcode, 0);
    if (WEXITSTATUS(exitcode)) return WEXITSTATUS(exitcode);
#else
    // Set up the environment
    SetEnvironmentVariable(_T("MWCIncludes"), MWCIncludes);
    SetEnvironmentVariable(_T("MWLibraries"), MWLibraries);
    SetEnvironmentVariable(_T("MWLibraryFiles"), MWLibraryFiles);

    // Execute the tool
    _TCHAR *argv_quoted = win_argv_build((const _TCHAR **)new_argv);
    if (args.wrap_dbg) fprintf(stderr, FMT_TS "\n", argv_quoted);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitcode;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    if (!CreateProcess(new_argv[0], argv_quoted, NULL, NULL, FALSE, 0,
            NULL, NULL, &si, &pi)) {
        fprintf(stderr, PROGRAM_NAME ": CreateProcess failed\n");
        exit(EXIT_FAILURE);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitcode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (exitcode) return exitcode;
    free(argv_quoted);
#endif

    if (wine) free(wine);
    free(tool);
    free(new_argv);
    free(tool_dir);

    free(MWCIncludes);
    free(MWLibraries);
    free(MWLibraryFiles);

    // Fix dependency file if generated
    _TCHAR *depfile = NULL;
    if (args.M) {
        // Only generated a depfile
        depfile = _tcsdup(args.o);
    } else if (args.MD) {
        // Generating a depfile as a side-effect of compilation
        _TCHAR *out = NULL;
        if (args.o) out = args.o;
        if (args.precompile) out = args.precompile;

        // Replace filename extension with .d
        if (out) {
            out = _tcsdup(out);
            _TCHAR *dot = _tcsrchr(out, '.');
            if (!dot) dot = _tcsrchr(out, '\0');
            *dot = '\0';

            size_t len = dot - out + 3;
            out = realloc(out, sizeof(*out) * len);
            _tcscat(out, _T(".d"));
        }
        depfile = out;
    }

    if (depfile) {
        if (args.wrap_dbg) {
            fprintf(stderr, "dep: " FMT_TS "\n", depfile);
            if (cfg.path_unx && cfg.path_win) {
                fprintf(stderr, "path_unx: %s\n", cfg.path_unx);
                fprintf(stderr, "path_win: %s\n", cfg.path_win);
            }
            if (cfg.path_build_unx && cfg.path_build_win) {
                fprintf(stderr, "path_build_unx: %s\n", cfg.path_build_unx);
                fprintf(stderr, "path_build_win: %s\n", cfg.path_build_win);
            }
        }
        fix_depfile(depfile,
            cfg.path_unx, cfg.path_win,
            cfg.path_build_unx, cfg.path_build_win);
        free(depfile);
    }

    cfg_free(cfg);
    return EXIT_SUCCESS;
}
