#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
extern char **environ;
#else
#include <windows.h>
#include <tchar.h>
#endif

#include "config.h"

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
#define _stat stat

#define _ftprintf(...) fprintf(__VA_ARGS__)
#define _tcscat(...) strcat(__VA_ARGS__)
#define _tcschr(...) strchr(__VA_ARGS__)
#define _tcscmp(...) strcmp(__VA_ARGS__)
#define _tcscspn(...) strcspn(__VA_ARGS__)
#define _tcsdup(...) strdup(__VA_ARGS__)
#define _tcslen(...) strlen(__VA_ARGS__)
#define _tcsrchr(...) strrchr(__VA_ARGS__)
#define _tfopen(...) fopen(__VA_ARGS__)
#define _tgetenv(...) getenv(__VA_ARGS__)
#define _tstat(...) stat(__VA_ARGS__)
#define _vsntprintf(...) vsnprintf(__VA_ARGS__)
#endif

#ifndef _WIN32
#define PATH_SEP "/"
#define PATH_DELIM "/"
#else
#define PATH_SEP "\\"
#define PATH_DELIM "/\\"
#endif

// TODO: Temporary hacks for pokeplatinum, remove whenever possible
#define PLAT_TEMP

enum libarch {
    LIBARCH_v4,
    LIBARCH_v4t,
    LIBARCH_v5,
    LIBARCH_v5t
};

struct args {
    enum libarch wrap_libarch;
    bool wrap_dbg;
    _TCHAR *wrap_ver;
    _TCHAR *wrap_sdk;
    _TCHAR *wrap_lib;
    _TCHAR *wrap_hack01;
#ifdef PLAT_TEMP
    bool wrap_noipa;
#endif
};

struct file {
    size_t size;
    unsigned char data[];
};

struct args args;

struct args parse_args(int argc, _TCHAR *argv[], int *out_argc, _TCHAR ***out_argv)
{
    struct args args = {
        .wrap_libarch = LIBARCH_v5
    };

    int new_argc = 0;
    _TCHAR **new_argv = malloc(sizeof(_TCHAR *) * (argc + 2));
    new_argv[new_argc++] = NULL;

    bool thumb = false;
    while (argc >= 1) {
        int skip = 0;
        int copy = 0;

        if (_tcscmp(argv[0], _T("-proc")) == 0 && argc >= 2) {
            if (_tcscmp(argv[1], _T("arm7tdmi")) == 0 ||
                    _tcscmp(argv[1], _T("arm4T")) == 0) {
                args.wrap_libarch = LIBARCH_v4;
            } else if (_tcscmp(argv[1], _T("arm946e")) == 0 ||
                    _tcscmp(argv[1], _T("arm5TE")) == 0) {
                args.wrap_libarch = LIBARCH_v5;
            }
            copy = 2;
        } else if (_tcscmp(argv[0], _T("-thumb")) == 0) {
            thumb = true;
            copy = 1;
        } else if (_tcscmp(argv[0], _T("-nothumb")) == 0) {
            thumb = false;
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
        } else if (_tcscmp(argv[0], _T("-wrap:lib")) == 0 && argc >= 2) {
            args.wrap_lib = argv[1];
            skip = 2;
        } else if (_tcscmp(argv[0], _T("-wrap:hack01")) == 0 && argc >= 2) {
            args.wrap_hack01 = argv[1];
            skip = 2;
#ifdef PLAT_TEMP
        } else if (_tcscmp(argv[0], _T("-wrap:noipa")) == 0) {
            args.wrap_noipa = true;
            skip = 1;
#endif
        } else {
            copy = 1;
        }

        argc -= skip + copy;
        while (skip--) argv++;
        while (copy--) new_argv[new_argc++] = *argv++;
    }
    new_argv[new_argc] = NULL;

    if (thumb) {
        if (args.wrap_libarch == LIBARCH_v4) args.wrap_libarch = LIBARCH_v4t;
        if (args.wrap_libarch == LIBARCH_v5) args.wrap_libarch = LIBARCH_v5t;
    }

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
    char *res = malloc(size * sizeof(char));
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
    _TCHAR *res = malloc(size * sizeof(_TCHAR));
    if (!res) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, str, -1, res, size);
    return res;
#else
    return strdup(str);
#endif
}

_TCHAR *strmake(const _TCHAR *format, ...)
{
    va_list ap;

    va_start(ap, format);
    int size = _vsntprintf(NULL, 0, format, ap) + 1;
    _TCHAR *dest = malloc(sizeof(_TCHAR) * size);
    _vsntprintf(dest, size, format, ap);
    va_end(ap);
    return dest;
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

// Figure out the directory that the program's running from
_TCHAR *find_self(const _TCHAR *argv0)
{
    _TCHAR *dir = _tcsdup(argv0);
    dir = my_dirname(dir);

    if (!*dir) {
#ifndef _WIN32
        ssize_t res;
        ssize_t size = 0;
        do {
            free(dir);
            dir = malloc(sizeof(*dir) * (size += 0x1000));
        } while ((res = readlink("/proc/self/exe", dir, size - 1)) == size - 1);
        if (res == -1) {
            perror(PROGRAM_NAME ": readlink");
            exit(EXIT_FAILURE);
        }
        dir[res] = '\0';
        dir = my_dirname(dir);
#else
        DWORD res;
        DWORD size = 0;
        do {
            free(dir);
            dir = malloc(sizeof(*dir) * (size += 0x1000));
        } while ((res = GetModuleFileName(NULL, dir, size)) == size);
        if (!res) {
            fprintf(stderr, PROGRAM_NAME ": GetModuleFileName failed\n");
            exit(EXIT_FAILURE);
        }
        dir = my_dirname(dir);
#endif
    }

    return dir;
}

_TCHAR *find_datadir(const _TCHAR *self)
{
    _TCHAR *dir = NULL;
    const _TCHAR *env = _tgetenv(_T("SKREW_DATADIR"));
    if (env) {
        dir = _tcsdup(env);
    } else if (*self) {
        dir = strmake(_T(FMT_TS PATH_SEP BIN_TO_DATADIR), self);
    }
    return dir;
}

_TCHAR *find_libdir(const _TCHAR *self)
{
    _TCHAR *dir = NULL;
    const _TCHAR *env = _tgetenv(_T("SKREW_LIBDIR"));
    if (env) {
        dir = _tcsdup(env);
    } else if (*self) {
        dir = strmake(_T(FMT_TS PATH_SEP BIN_TO_LIBDIR), self);
    }
    return dir;
}

_TCHAR *sdk_version(const _TCHAR *datadir, const _TCHAR *ver, const _TCHAR *tool, const _TCHAR *suff)
{
    if (!datadir) return NULL;

    // Compatibility: try multiple subdirs
    _TCHAR *subdirs[] = {
        _T("sdk"),
        _T("sdk" PATH_SEP "ds"),
        NULL
    };

    struct file *file = NULL;
    for (_TCHAR **dir = subdirs; *dir; dir++) {
        _TCHAR *path = strmake(
            _T(FMT_TS PATH_SEP FMT_TS PATH_SEP FMT_TS PATH_SEP FMT_TS FMT_TS
                ".txt"),
            datadir, *dir, ver, tool, suff);

        if (args.wrap_dbg) {
            fprintf(stderr, "sdk: " FMT_TS "\n", path);
        }

        file = file_read(path);
        free(path);
        if (file) break;
    }
    if (!file) return NULL;

    // Strip the end and start of the string
    unsigned char *s = file->data;
    unsigned char *e = file->data + file->size - 1;
    while (s <  e && (!*s || strchr(" \t\n", *s))) s++;
    while (e >= s && (!*e || strchr(" \t\n", *e))) e--;
    e++;

    size_t size = e - s;
    char *utf = malloc(size + 1);
    memcpy(utf, s, size);
    utf[size] = '\0';
    free(file);

    _TCHAR *res = utftotc(utf);
    free(utf);
    return res;
}

_TCHAR *lib_version(const _TCHAR *libdir, const _TCHAR *ver)
{
    if (!libdir) return NULL;

    // Compatibility: try multiple subdirs
    _TCHAR *subdirs[] = {
        _T("sdk"),
        _T("sdk" PATH_SEP "ds"),
        NULL
    };

    _TCHAR *path = NULL;
    for (_TCHAR **dir = subdirs; *dir; dir++) {
        path = strmake(
            _T(FMT_TS PATH_SEP FMT_TS PATH_SEP FMT_TS),
            libdir, *dir, ver);

        if (args.wrap_dbg) {
            fprintf(stderr, "lib: " FMT_TS "\n", path);
        }

        // Check if the directory exists
        struct _stat buf;
        if (_tstat(path, &buf) != -1) {
            if (S_ISDIR(buf.st_mode)) break;
        }

        free(path); path = NULL;
    }
    return path;
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

void setenv_weak(const _TCHAR *name, const _TCHAR *value)
{
    if (!value) return;
#ifndef _WIN32
    setenv(name, value, false);
#else
    if (!_tgetenv(name)) SetEnvironmentVariable(name, value);
#endif
}

#ifdef PLAT_TEMP
void remove_ipa_arguments(int *argc, _TCHAR **argv)
{
    int new_argc = 0;
    for (int i = 0; i < *argc; i++) {
        if (argv[i] && _tcscmp(argv[i], _T("-ipa")) == 0 && (i + 1) < *argc) {
            // Skip the current "-ipa" argument and the following argument.
            i++; // Increment to skip the next argument.
        } else {
            // Keep this argument, by moving it down if necessary.
            argv[new_argc] = argv[i];
            new_argc++;
        }
    }

    // Update the original argc to the new count of arguments.
    *argc = new_argc;
    argv[*argc] = NULL;
}
#endif

int _tmain(int argc, _TCHAR *argv[])
{
#ifndef WRAP_PROG
#define MIN_ARGS 2
#else
#define MIN_ARGS 1
#endif

    if (argc < MIN_ARGS) {
        fprintf(stderr, PROGRAM_NAME ": Too few arguments\n");
        return EXIT_FAILURE;
    }

    // Filter the arguments to pass to the application
    int new_argc;
    _TCHAR **new_argv;
    args = parse_args(argc - MIN_ARGS, argv + MIN_ARGS, &new_argc, &new_argv);

#ifdef PLAT_TEMP
    if (args.wrap_noipa) {
        remove_ipa_arguments(&new_argc, new_argv);
    }
#endif

    // Figure out program location
    _TCHAR *tool_dir = find_self(argv[0]);
    _TCHAR *datadir = find_datadir(tool_dir);
    _TCHAR *libdir = find_libdir(tool_dir);

#ifndef WRAP_PROG
    const _TCHAR *tool_bin = argv[1];
#else
    const _TCHAR *tool_bin = _T(WRAP_PROG);
#endif

    const _TCHAR *tool_ver = NULL;
    const _TCHAR *tool_sdk = NULL;
    const _TCHAR *tool_lib = NULL;
    enum libarch tool_libarch = args.wrap_libarch;

    if (args.wrap_ver) tool_ver = args.wrap_ver;
    if (args.wrap_sdk) tool_sdk = args.wrap_sdk;
    if (args.wrap_lib) tool_lib = args.wrap_lib;

    // Figure out the tool filename
    _TCHAR *tool_file = NULL;
    if (tool_ver) {
        tool_file = strmake(_T(FMT_TS "-" FMT_TS ".exe"), tool_bin, tool_ver);
    } else if (tool_sdk) {
        tool_file = sdk_version(datadir, tool_sdk, tool_bin, _T(".exe"));
        if (!tool_file) {
            fprintf(stderr, PROGRAM_NAME ": did not find SDK version "
                FMT_TS "\n", tool_sdk);
            exit(EXIT_FAILURE);
        }
    } else {
        // If no version was specified, pick a default for generic binary names
        _TCHAR *ver = _T("");
        if (_tcscmp(tool_bin, _T("mwccarm")) == 0) {
            ver = _T("-" DEFAULT_MWCCARM);
        }
        if (_tcscmp(tool_bin, _T("mwldarm")) == 0) {
            ver = _T("-" DEFAULT_MWLDARM);
        }
        if (_tcscmp(tool_bin, _T("mwasmarm")) == 0) {
            ver = _T("-" DEFAULT_MWASMARM);
        }
        tool_file = strmake(_T(FMT_TS FMT_TS ".exe"), tool_bin, ver);
    }

    // Make a path of the chosen tool
    _TCHAR *tool = strmake(_T(FMT_TS FMT_TS FMT_TS),
        tool_dir, *tool_dir ? _T(PATH_SEP) : _T(""), tool_file);
    new_argv[0] = tool;

    free(tool_file);
    free(tool_dir);

    // Build standard library paths for environment variables
    _TCHAR *tool_libver = NULL;
    if (tool_lib) {
        tool_libver = _tcsdup(tool_lib);
    } else if (tool_sdk) {
        tool_libver = sdk_version(datadir, tool_sdk, _T("lib"), _T(""));
    }

    _TCHAR *tool_libdir = NULL;
    if (tool_libver) {
        tool_libdir = lib_version(libdir, tool_libver);
        free(tool_libver);
    }

    free(datadir);
    free(libdir);

    _TCHAR *MWCIncludes = NULL;
    _TCHAR *MWLibraries = NULL;
    if (tool_libdir) {
        MWCIncludes = strmake(_T(
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS),
            tool_libdir, _T("msl" PATH_SEP "MSL_C" PATH_SEP "MSL_ARM"
                PATH_SEP "Include"),
            tool_libdir, _T("msl" PATH_SEP "MSL_C" PATH_SEP "MSL_Common"
                PATH_SEP "Include"),
            tool_libdir, _T("msl" PATH_SEP "MSL_C" PATH_SEP "MSL_Common_Embedded"
                PATH_SEP "Math" PATH_SEP "Include"),
            tool_libdir, _T("msl" PATH_SEP "MSL_C++" PATH_SEP "MSL_ARM"
                PATH_SEP "Include"),
            tool_libdir, _T("msl" PATH_SEP "MSL_C++" PATH_SEP "MSL_Common"
                PATH_SEP "Include"),
            tool_libdir, _T("msl" PATH_SEP "MSL_Extras" PATH_SEP "MSL_Common"
                PATH_SEP "Include"),
            tool_libdir, _T("Profiler" PATH_SEP "include"),
            tool_libdir, _T("Runtime" PATH_SEP "Runtime_ARM" PATH_SEP "Runtime_NITRO"
                PATH_SEP "Common_Includes"),
            tool_libdir, _T("msl" PATH_SEP "MSL_Extras" PATH_SEP "MSL_ARM"
                PATH_SEP "Include"));

        MWLibraries = strmake(_T(
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS ";"
            FMT_TS PATH_SEP FMT_TS),
            tool_libdir, _T("msl" PATH_SEP "MSL_C" PATH_SEP "MSL_ARM"
                PATH_SEP "Lib"),
            tool_libdir, _T("msl" PATH_SEP "MSL_C++" PATH_SEP "MSL_ARM"
                PATH_SEP "Lib"),
            tool_libdir, _T("msl" PATH_SEP "MSL_Extras" PATH_SEP "MSL_ARM"
                PATH_SEP "Lib"),
            tool_libdir, _T("Runtime" PATH_SEP "Runtime_ARM" PATH_SEP "Runtime_NITRO"
                PATH_SEP "Lib"),
            tool_libdir, _T("Mathlib" PATH_SEP "lib"));
    }

    const _TCHAR *MWLibraryFiles = NULL;
    switch (tool_libarch) {
    case LIBARCH_v4:
        MWLibraryFiles = _T(
            "MSL_C_NITRO_Ai_LE.a" ";"
            "MSL_Extras_NITRO_Ai_LE.a" ";"
            "MSL_CPP_NITRO_Ai_LE.a" ";"
            "FP_fastI_v4t_LE.a" ";"
            "NITRO_Runtime_Ai_LE.a");
        break;

    case LIBARCH_v4t:
        MWLibraryFiles = _T(
            "MSL_C_NITRO_T_LE.a" ";"
            "MSL_Extras_NITRO_T_LE.a" ";"
            "MSL_CPP_NITRO_T_LE.a" ";"
            "FP_fastI_v4t_LE.a" ";"
            "NITRO_Runtime_T_LE.a");
        break;

    case LIBARCH_v5:
        MWLibraryFiles = _T(
            "MSL_C_NITRO_Ai_LE.a" ";"
            "MSL_Extras_NITRO_Ai_LE.a" ";"
            "MSL_CPP_NITRO_Ai_LE.a" ";"
            "FP_fastI_v5t_LE.a" ";"
            "NITRO_Runtime_Ai_LE.a");
        break;

    case LIBARCH_v5t:
        MWLibraryFiles = _T(
            "MSL_C_NITRO_T_LE.a" ";"
            "MSL_Extras_NITRO_T_LE.a" ";"
            "MSL_CPP_NITRO_T_LE.a" ";"
            "FP_fastI_v5t_LE.a" ";"
            "NITRO_Runtime_T_LE.a");
        break;
    }

    if (args.wrap_dbg) {
        if (MWCIncludes) {
            fprintf(stderr, "MWCIncludes: " FMT_TS "\n", MWCIncludes);
        }
        if (MWLibraries) {
            fprintf(stderr, "MWLibraries: " FMT_TS "\n", MWLibraries);
        }
        if (MWLibraryFiles) {
            fprintf(stderr, "MWLibraryFiles: " FMT_TS "\n", MWLibraryFiles);
        }
    }

    // Set up the environment
    setenv_weak(_T("MWCIncludes"), MWCIncludes);
    setenv_weak(_T("MWLibraries"), MWLibraries);
    setenv_weak(_T("MWLibraryFiles"), MWLibraryFiles);
    setenv_weak(_T("SKREW_HACK01"), args.wrap_hack01);

#ifndef _WIN32
    // Execute the tool
    if (args.wrap_dbg) {
        for (int x = 0; x < new_argc; x++) {
            printf(x ? " %s" : "%s", new_argv[x]);
        }
        printf("\n");
    }
    pid_t pid;
    if (posix_spawn(&pid, tool, NULL, NULL, new_argv, environ) != 0) {
        perror(PROGRAM_NAME ": posix_spawn");
        exit(EXIT_FAILURE);
    }
    int exitcode;
    waitpid(pid, &exitcode, 0);
    if (WIFSIGNALED(exitcode)) return 128 + WTERMSIG(exitcode);
    if (WIFEXITED(exitcode)) return WEXITSTATUS(exitcode);
#else
    // Execute the tool
    _TCHAR *argv_quoted = win_argv_build((const _TCHAR **)new_argv);
    if (args.wrap_dbg) fprintf(stderr, FMT_TS "\n", argv_quoted);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitcode;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    if (!CreateProcess(tool, argv_quoted, NULL, NULL, FALSE, 0,
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
    free(MWCIncludes);
    free(MWLibraries);

    free(tool);
    free(new_argv);
    return EXIT_SUCCESS;
}
