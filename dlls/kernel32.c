#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

// WINE headers, used for the sake of type-checking definitions
#define WINBASEAPI
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winnt.h"
#include "consoleapi.h"
#include "wincon.h"
#include "winnls.h"
#include "timezoneapi.h"

// Local headers
#include "_utils.h"

// winbase.h

VOID DECLSPEC_NORETURN WINAPI ExitProcess(DWORD uExitCode)
{
    TRACE("ExitProcess: uExitCode=%lu", uExitCode);
    exit(uExitCode);
}

BOOL WINAPI IsBadReadPtr(LPCVOID,UINT_PTR);

HANDLE WINAPI GetCurrentProcess(void)
{
    TRACE("GetCurrentProcess");
    return (HANDLE)-1;
}

BOOL WINAPI DuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, HANDLE *lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    uintptr_t uSourceHandle = (uintptr_t)hSourceHandle;

    if (uSourceHandle < 1 || uSourceHandle > 3) goto die;

    *lpTargetHandle = hSourceHandle;
    TRACE("DuplicateHandle: hSourceHandle=%d", uSourceHandle);
    return TRUE;

die:
    DIE("DuplicateHandle: %p %p %p %lx %d %lx",
        hSourceProcessHandle, hSourceHandle, hTargetProcessHandle,
        dwDesiredAccess, bInheritHandle, dwOptions);
}

DWORD WINAPI my_GetLastError(void)
{
    DWORD res = errno;
    TRACE("GetLastError: res=%ld", res);
    return res;
}

HANDLE WINAPI GetStdHandle(DWORD nStdHandle)
{
    DWORD num = -nStdHandle - 10;
    if (num > 2) return INVALID_HANDLE_VALUE;
    HANDLE res = (HANDLE)(num + 1);
    TRACE("GetStdHandle: res=%ld nStdHandle=%ld", (DWORD)res, nStdHandle);
    return res;
}

#ifndef _WIN32
// Assuming no multithreading
void WINAPI InitializeCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
void WINAPI DeleteCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
void WINAPI EnterCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
void WINAPI LeaveCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
#endif

DWORD GetFileAttributes_do(char *path)
{
    struct stat buf;
    if (stat(path, &buf) != 0) return INVALID_FILE_ATTRIBUTES;

    DWORD res = 0;
    res |= (buf.st_mode & S_IFMT) == S_IFDIR ?
        FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
    return res;
}

struct findfile {
    DIR *dir;
    char *path;
};

BOOL FindNextFileA_do(struct findfile *findfile, LPWIN32_FIND_DATAA lpFindFileData)
{
    for (;;) {
        char *full;
        if (findfile->dir) {
            struct dirent *buf = readdir(findfile->dir);
            if (!buf) return FALSE;

            // Join path
            size_t len_path = strlen(findfile->path);
            size_t len_file = strlen(buf->d_name);
            full = malloc(len_path + 1 + len_file + 1);
            memcpy(full, findfile->path, len_path);
            full[len_path] = '/';
            memcpy(full + len_path + 1, buf->d_name, len_file);
            full[len_path + 1 + len_file] = '\0';
        } else {
            if (!findfile->path) return FALSE;
            full = findfile->path;
            findfile->path = NULL;
        }

        // If the file was removed between opendir() and now, skip it
        DWORD attrs = GetFileAttributes_do(full);
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            free(full);
            if (errno == ENOENT) continue;
            else DIE("FindNextFileA_do: unexpected errno: %d", errno);
        }

        // Get filename component
        char *file = strrchr(full, '/');
        file = file ? file + 1 : full;
        size_t len_file = strlen(file);
        if (len_file > MAX_PATH - 1) len_file = MAX_PATH - 1;

        // Initialize WIN32_FIND_DATAA
        memset(lpFindFileData, 0, sizeof(*lpFindFileData));
        lpFindFileData->dwFileAttributes = attrs;
        memcpy(lpFindFileData->cFileName, file, len_file);
        lpFindFileData->cFileName[len_file] = '\0';

        DEBUG("FindNextFileA_do: '%s' (0x%lx)", file, attrs);
        free(full);
        return TRUE;
    }
}

HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    STUB("FindFirstFileA: only filename and attributes: '%s'", lpFileName);
    HANDLE res = INVALID_HANDLE_VALUE;
#ifdef TRACE
    char *path = path_dup_unx(lpFileName);
#endif

    DIR *dir = NULL;

    // Check if we're listing a directory
    char *dir_path = path_dup_unx(lpFileName);
    size_t len_path = strlen(dir_path);
    if (dir_path[len_path - 2] == '/' && dir_path[len_path - 1] == '*') {
        dir_path[len_path - 2] = '\0';

        // Open directory
        dir = opendir(dir_path);
        if (!dir) {
            free(dir_path);
            goto end;
        }
    }

    // Create findfile object
    struct findfile *findfile = malloc(sizeof(struct findfile));
    findfile->path = dir_path;
    findfile->dir = dir;

    // Find the first file
    if (!FindNextFileA_do(findfile, lpFindFileData)) {
        if (findfile->dir) closedir(findfile->dir);
        if (findfile->path) free(findfile->path);
        free(findfile);
        goto end;
    }

    res = (HANDLE)findfile;
end:
    TRACE("FindFirstFileA: res=%p lpFileName='%s'", res, path);
#ifdef TRACE
    free(path);
#endif
    return res;
}

DWORD WINAPI my_GetFileAttributesA(LPCSTR lpFileName)
{
    STUB("GetFileAttributes: only presence and directory: '%s'", lpFileName);
    char *path = path_dup_unx(lpFileName);
    DWORD res = GetFileAttributes_do(path);
    DEBUG("GetFileAttributesA: res=%lx lpFileName='%s'", res, path);
    free(path);
    return res;
}

BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    STUB("FindNextFileA: only filename and attributes");
    struct findfile *findfile = (struct findfile *)hFindFile;

    BOOL res = FindNextFileA_do(findfile, lpFindFileData);
    TRACE("FindNextFileA: res=%d hFindFile=%p", res, hFindFile);
    return res;
}

BOOL WINAPI FindClose(HANDLE hFindFile)
{
    struct findfile *findfile = (struct findfile *)hFindFile;

    if (findfile->dir) closedir(findfile->dir);
    if (findfile->path) free(findfile->path);

    BOOL res = TRUE;
    TRACE("FindClose: res=%d hFindFile=%p", res, hFindFile);
    free(findfile);
    return res;
}

#undef GetEnvironmentStrings
LPSTR WINAPI GetEnvironmentStringsA(void)
{
    STUB("GetEnvironmentStringsA");
    return "\0";
}

LPSTR WINAPI GetEnvironmentStrings(void)
{
    return GetEnvironmentStringsA();
}

BOOL WINAPI FreeEnvironmentStringsA(LPSTR penv)
{
    (void)penv;
    STUB("FreeEnvironmentStringsA");
    return TRUE;
}

UINT WINAPI GetCurrentDirectoryA(UINT nBufferLength, LPSTR lpBuffer)
{
    char path[0x1000];
    if (!getcwd(path, sizeof(path))) return 0;

    size_t path_len = strlen(path) + 1;
    if (nBufferLength < path_len) return path_len;

    // Copy to DOS path
    char *p = lpBuffer;
    for (char *c = path; *c; c++) *p++ = *c == '/' ? '\\' : *c;
    *p++ = '\0';

    DEBUG("GetCurrentDirectoryA: '%s'", lpBuffer);
    return strlen(lpBuffer);
}

BOOL WINAPI CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
DWORD WINAPI WaitForSingleObject(HANDLE,DWORD);
BOOL WINAPI GetExitCodeProcess(HANDLE,LPDWORD);

BOOL WINAPI CloseHandle(HANDLE hObject)
{
    uintptr_t uObject = (uintptr_t)hObject;
    BOOL res = TRUE;
    if (uObject > 3) res = close(uObject - 1) == 0;
    TRACE("CloseHandle: res=%d hObject=%p", res, hObject);
    return res;
}

#ifndef _WIN32
// Assuming no multithreading
DWORD WINAPI TlsAlloc(void)
{
    LPVOID *mem = malloc(sizeof(LPVOID));
    *mem = 0;
    return (DWORD)mem;
}

BOOL WINAPI TlsFree(DWORD dwTlsIndex)
{
    free((LPVOID *)dwTlsIndex);
    return TRUE;
}

LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex)
{
    return *(LPVOID *)dwTlsIndex;
}

BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    *(LPVOID *)dwTlsIndex = lpTlsValue;
    return TRUE;
}
#endif

HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName)
{
    (void)lpModuleName;
    STUB("GetModuleHandleA: lpModuleName='%s'", lpModuleName);
    return NULL;
}

DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFileName, DWORD nSize)
{
    (void)hModule;
    (void)lpFileName;
    (void)nSize;
    STUB("GetModuleFileNameA: hModule=%p", hModule);
    return 0;
}

HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName)
{
    (void)lpLibFileName;
    STUB("HACK: LoadLibraryA");
    HANDLE res = (HANDLE)-1;
    TRACE("LoadLibraryA: res=%p lpLibFileName='%s'", res, lpLibFileName);
    return res;
}

BOOL WINAPI FreeLibrary(HMODULE hLibModule)
{
    (void)hLibModule;
    STUB("HACK: FreeLibrary");
    BOOL res = TRUE;
    TRACE("FreeLibrary: res=%d hLibModule=%p", res, hLibModule);
    return res;
}

struct alloc {
    size_t size;
    UINT flags;
    char data[];
};

HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes)
{
    struct alloc *alloc = malloc(sizeof(struct alloc) + dwBytes);
    alloc->size = dwBytes;
    alloc->flags = uFlags;

    // Optionally initialize memory
    if (uFlags & GMEM_ZEROINIT) {
        uFlags &= ~GMEM_ZEROINIT;
        memset(alloc->data, 0, alloc->size);
    }

    // Don't do anything with GMEM_MOVEABLE
    uFlags &= ~GMEM_MOVEABLE;

    // Unhandled flags
    if (uFlags) {
        DIE("GlobalAlloc: Unhandled flags: %x", uFlags);
    }

    return alloc->data;
}

HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    free((struct alloc *)hMem - 1);
    return NULL;
}

DWORD WINAPI GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart)
{
    DWORD res = 0;

    size_t name_len = strlen(lpFileName);
    size_t full_len = name_len;

    // If the path isn't absolute, add the current directory
    if (lpFileName[0] != '\\' && !path_has_drv(lpFileName)) {
        res = GetCurrentDirectoryA(nBufferLength, lpBuffer);
        if (!res) return 0;
        full_len += res + 1;
        if (res + 1 > nBufferLength) return full_len + 1;
        lpBuffer[res++] = '\\';
    }

    // Append the filename
    if (full_len + 1 > nBufferLength) return full_len + 1;
    strcpy(lpBuffer + res, lpFileName);

    // Figure out the file portion
    char *part = strrchr(lpBuffer, '\\');
    part = part ? part + 1 : lpBuffer;
    *lpFilePart = part;

    DEBUG("GetFullPathNameA: '%s' = '%s' (%s)", lpFileName, lpBuffer, *lpFilePart);
    return full_len;
}

DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpDistanceToMoveHigh) {
        DIE("SetFilePointer: No support for lpDistanceToMoveHigh");
    }

    DWORD res = lseek(uFile - 1, lDistanceToMove, dwMoveMethod);
    TRACE("SetFilePointer: res=%ld hFile=%p lDistanceToMove=%ld dwMoveMethod=%ld",
        res, hFile, lDistanceToMove, dwMoveMethod);
    return res;
}

BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpOverlapped) {
        DIE("WriteFile: No support for lpOverlapped");
    }

    int count = write(uFile - 1, lpBuffer, nNumberOfBytesToWrite);
    BOOL res = count != -1;
    if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = count != -1 ? count : 0;
    TRACE("WriteFile: res=%d hFile=%p nNumberOfBytesToWrite=%ld "
        "lpNumberOfBytesWritten=%ld", res, hFile, nNumberOfBytesToWrite,
        lpNumberOfBytesWritten ? *lpNumberOfBytesWritten : 0);
    return res;
}

BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpOverlapped) {
        DIE("ReadFile: No support for lpOverlapped");
    }

    int count = read(uFile - 1, lpBuffer, nNumberOfBytesToRead);
    BOOL res = count != -1;
    if (lpNumberOfBytesRead) *lpNumberOfBytesRead = count != -1 ? count : 0;
    TRACE("ReadFile: res=%d hFile=%p nNumberOfBytesToRead=%ld "
        "lpNumberOfBytesRead=%ld", res, hFile, nNumberOfBytesToRead,
        lpNumberOfBytesRead ? *lpNumberOfBytesRead : 0);
    return res;
}

HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    // Cover for unsupported functionality
    //if (dwShareMode) goto die; // Ignored, can't action on this
    if (lpSecurityAttributes) goto die;
    if (dwFlagsAndAttributes & ~FILE_ATTRIBUTE_NORMAL) goto die;
    if (hTemplateFile) goto die;

    // Figure out the flags
    if (!dwCreationDisposition || dwCreationDisposition > 6) goto die;
    int flags = (int []){
        O_CREAT | O_EXCL,  // CREATE_NEW
        O_CREAT | O_TRUNC,  // CREATE_ALWAYS
        0,  // OPEN_EXISTING
        O_CREAT,  // OPEN_ALWAYS
        O_TRUNC  // TRUNCATE_EXISTING
    }[dwCreationDisposition - 1];

    switch (dwDesiredAccess) {
    case GENERIC_WRITE: flags |= O_WRONLY; break;
    case GENERIC_READ: flags |= O_RDONLY; break;
    case GENERIC_WRITE | GENERIC_READ: flags |= O_RDWR; break;
    default: goto die;
    }

#ifdef _WIN32
    flags |= O_BINARY;
#endif

    char *path = path_dup_unx(lpFileName);
    HANDLE res = (HANDLE)(open(path, flags, 0666) + 1);
    DEBUG("CreateFileA: res=%p lpFileName='%s'", res, path);
    free(path);
    return res;

die:
    DIE("CreateFileA: '%s' %lx %lx %p %lx %lx %p",
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
    );
}

DWORD WINAPI GetTickCount(void)
{
    STUB("GetTickCount");
    return 0;
}

BOOL WINAPI DeleteFileA(LPCSTR lpFileName)
{
    char *path = path_dup_unx(lpFileName);
    BOOL res = unlink(path) == 0;
    DEBUG("DeleteFileA: res=%d lpFileName='%s'", res, path);
    free(path);
    return res;
}

BOOL WINAPI MoveFileA(LPCSTR,LPCSTR);
DWORD WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,va_list*);

BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
    (void)hFile;
    (void)lpCreationTime;
    (void)lpLastAccessTime;
    (void)lpLastWriteTime;
    STUB("GetFileTime: hFile=%p", hFile);
    return FALSE;
}

BOOL WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);

DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpFileSizeHigh) {
        DIE("GetFileSize: No support for lpFileSizeHigh");
    }

    struct stat buf;
    if (fstat(uFile - 1, &buf) != 0) return INVALID_FILE_SIZE;
    DWORD res = buf.st_size;
    TRACE("GetFileSize: res=%ld hFile=%d", res, uFile);
    return res;
}

BOOL WINAPI SetEndOfFile(HANDLE);
BOOL WINAPI CreateDirectoryA(LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL WINAPI RemoveDirectoryA(LPCSTR);
BOOL WINAPI SetStdHandle(DWORD,HANDLE);

VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetSystemTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
    (void)lpSystemTime;
    STUB("SystemTimeToFileTime");
    memset(lpFileTime, 0, sizeof(*lpFileTime));
    return FALSE;
}

INT WINAPI CompareFileTime(const FILETIME*,const FILETIME*);

HGLOBAL WINAPI GlobalReAlloc(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags)
{
    struct alloc *alloc = realloc((struct alloc *)hMem - 1,
        sizeof(struct alloc) + dwBytes);

    size_t size_old = alloc->size;
    alloc->size = dwBytes;
    alloc->flags = uFlags;

    // Optionally initialize memory
    if (uFlags & GMEM_ZEROINIT) {
        uFlags &= ~GMEM_ZEROINIT;
        if (alloc->size > size_old) {
            memset(alloc->data + size_old, 0, alloc->size - size_old);
        }
    }

    // Don't do anything with GMEM_MOVEABLE
    uFlags &= ~GMEM_MOVEABLE;

    // Unhandled flags
    if (uFlags) {
        DIE("GlobalReAlloc: Unhandled flags: %x", uFlags);
    }

    return alloc->data;
}

UINT WINAPI GlobalFlags(HGLOBAL hMem)
{
    return ((struct alloc *)hMem - 1)->flags;
}

BOOL WINAPI FileTimeToSystemTime(const FILETIME*,LPSYSTEMTIME);
HRSRC WINAPI FindResourceA(HMODULE,LPCSTR,LPCSTR);
HGLOBAL WINAPI LoadResource(HMODULE,HRSRC);
LPVOID WINAPI LockResource(HGLOBAL);
DWORD WINAPI SizeofResource(HMODULE,HRSRC);

HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
    (void)hFile;
    (void)lpFileMappingAttributes;
    (void)flProtect;
    (void)dwMaximumSizeHigh;
    (void)dwMaximumSizeLow;
    (void)lpName;
    STUB("CreateFileMappingA: hFile=%p flProtext=%lx dwMaximumSizeHigh=%ld"
        " dwMaximumSizeLow=%ld lpName='%s'",
        hFile, flProtect, dwMaximumSizeHigh,
        dwMaximumSizeLow, lpName ? lpName : "");
    return NULL;
}

LPVOID WINAPI MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,SIZE_T);
BOOL WINAPI UnmapViewOfFile(LPCVOID);

UINT WINAPI GetSystemDirectoryA(LPSTR lpBuffer, UINT uSize)
{
    (void)lpBuffer;
    (void)uSize;
    STUB("GetSystemDirectoryA");
    return 0;
}

UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize)
{
    (void)lpBuffer;
    (void)uSize;
    STUB("GetWindowsDirectoryA");
    return 0;
}

BOOL WINAPI SetFileAttributesA(LPCSTR,DWORD);
HANDLE WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);

VOID WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetLocalTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

WINBASEAPI BOOL        WINAPI LocalFileTimeToFileTime(const FILETIME*,LPFILETIME);

// winnt.h

NTSYSAPI void NTAPI RtlUnwind(void*,void*,struct _EXCEPTION_RECORD*,void*);

// consoleapi.h

BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE HandlerRoutine, BOOL Add)
{
    (void)HandlerRoutine;
    (void)Add;
    STUB("SetConsoleCtrlHandler");
    return FALSE;
}

// wincon.h

BOOL WINAPI GetConsoleScreenBufferInfo(HANDLE hConsole, LPCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo)
{
    (void)hConsole;
    (void)lpConsoleScreenBufferInfo;
    STUB("GetConsoleScreenBufferInfo: hConsole=%p", hConsole);
    return FALSE;
}

// winnls.h

INT WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
BOOL WINAPI IsValidCodePage(UINT);
UINT WINAPI GetACP(void);

// timezoneapi.h

DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{
    STUB("GetTimeZoneInformation");
    memset(lpTimeZoneInformation, 0, sizeof(*lpTimeZoneInformation));
    return TIME_ZONE_ID_INVALID;
}
