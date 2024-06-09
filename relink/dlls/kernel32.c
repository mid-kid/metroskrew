#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// WINE headers, used for the sake of type-checking definitions
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winnt.h"
#include "consoleapi.h"
#include "wincon.h"
#include "winnls.h"
#include "timezoneapi.h"

// Local headers
#include "utils.h"

// winbase.h

WINBASEAPI VOID DECLSPEC_NORETURN WINAPI ExitProcess(DWORD uExitCode)
{
    TR("ExitProcess: uExitCode=%lu", uExitCode);
    exit(uExitCode);
}

WINBASEAPI BOOL WINAPI IsBadReadPtr(LPCVOID,UINT_PTR);

WINBASEAPI HANDLE WINAPI GetCurrentProcess(void)
{
    TR("GetCurrentProcess");
    return (HANDLE)-1;
}

WINBASEAPI BOOL WINAPI DuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, HANDLE *lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    uintptr_t uSourceHandle = (uintptr_t)hSourceHandle;

    // TODO: Actually duplicate non-stdio handles
    if (uSourceHandle < 1 || uSourceHandle > 3) goto die;

    *lpTargetHandle = hSourceHandle;
    TR("DuplicateHandle: hSourceHandle=%d", uSourceHandle);
    return TRUE;

die:
    DIE("DuplicateHandle: %p %p %p %lx %d %lx",
        hSourceProcessHandle, hSourceHandle, hTargetProcessHandle,
        dwDesiredAccess, bInheritHandle, dwOptions);
}

#ifndef _WIN32
WINBASEAPI DWORD WINAPI GetLastError(void)
{
    DWORD res = errno;
    TR("GetLastError: res=%ld", res);
    return res;
}
#else
ALIAS("GetLastError", 0);
#endif

WINBASEAPI HANDLE WINAPI GetStdHandle(DWORD nStdHandle)
{
    DWORD num = -nStdHandle - 10;
    if (num > 2) return INVALID_HANDLE_VALUE;
    HANDLE res = (HANDLE)(num + 1);
    TR("GetStdHandle: res=%ld nStdHandle=%ld", (DWORD)res, nStdHandle);
    return res;
}

#ifndef _WIN32
// Assuming no multithreading
WINBASEAPI void WINAPI InitializeCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
WINBASEAPI void WINAPI DeleteCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
WINBASEAPI void WINAPI EnterCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
WINBASEAPI void WINAPI LeaveCriticalSection(CRITICAL_SECTION *lpCrit)
{ (void)lpCrit; }
#else
ALIAS("InitializeCriticalSection", 4);
ALIAS("DeleteCriticalSection", 4);
ALIAS("EnterCriticalSection", 4);
ALIAS("LeaveCriticalSection", 4);
#endif

WINBASEAPI HANDLE WINAPI FindFirstFileA(LPCSTR,LPWIN32_FIND_DATAA);

DWORD GetFileAttributes_do(char *path)
{
    struct stat buf;
    if (stat(path, &buf) != 0) return INVALID_FILE_ATTRIBUTES;

    DWORD res = 0;
    res |= (buf.st_mode & S_IFMT) == S_IFDIR ?
        FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
    return res;
}

#ifndef _WIN32
WINBASEAPI DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName)
{
    char *path = path_dup_unx_c(lpFileName);
    STUB("GetFileAttributes: only presence and directory: %s", path);
    DWORD res = GetFileAttributes_do(path);
    DB("GetFileAttributesA: res=%lx lpFileName=%s", res, path);
    free(path);
    return res;
}
#else
ALIAS("GetFileAttributesA", 4);
#endif

WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE,LPWIN32_FIND_DATAA);
WINBASEAPI BOOL WINAPI FindClose(HANDLE);
WINBASEAPI LPSTR WINAPI GetCommandLineA(void);

#ifndef _WIN32
#undef GetEnvironmentStrings
WINBASEAPI LPSTR WINAPI GetEnvironmentStrings(void)
    __attribute__((weak, alias("GetEnvironmentStringsA")));

WINBASEAPI LPSTR WINAPI GetEnvironmentStringsA(void)
{
    return "\0";
}

WINBASEAPI BOOL WINAPI FreeEnvironmentStringsA(LPSTR penv)
{
    (void)penv;
    return TRUE;
}
#else
ALIAS("GetEnvironmentStrings", 0);
ALIAS("FreeEnvironmentStringsA", 4);
#endif

#ifndef _WIN32
WINBASEAPI UINT WINAPI GetCurrentDirectoryA(UINT nBufferLength, LPSTR lpBuffer)
{
    char path[0x1000];
    if (!getcwd(path, sizeof(path))) return 0;

    size_t path_len = strlen(path) + 2 + 1;
    if (nBufferLength < path_len) return path_len;

    // Copy to DOS path
    char *p = lpBuffer;
    *p++ = 'Z';
    *p++ = ':';
    for (char *c = path; *c; c++) *p++ = *c == '/' ? '\\' : *c;
    *p++ = '\0';

    DB("GetCurrentDirectoryA: %s", lpBuffer);
    return strlen(lpBuffer);
}
#else
ALIAS("GetCurrentDirectoryA", 8);
#endif

WINBASEAPI BOOL WINAPI CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
WINBASEAPI DWORD WINAPI WaitForSingleObject(HANDLE,DWORD);
WINBASEAPI BOOL WINAPI GetExitCodeProcess(HANDLE,LPDWORD);

WINBASEAPI BOOL WINAPI CloseHandle(HANDLE hObject)
{
    uintptr_t uObject = (uintptr_t)hObject;
    BOOL res = TRUE;
    if (uObject > 3) res = close(uObject - 1) == 0;
    TR("CloseHandle: res=%d hObject=%p", res, hObject);
    return res;
}

#ifndef _WIN32
// Assuming no multithreading
WINBASEAPI DWORD WINAPI TlsAlloc(void)
{
    LPVOID *mem = malloc(sizeof(LPVOID));
    *mem = 0;
    return (DWORD)mem;
}

WINBASEAPI BOOL WINAPI TlsFree(DWORD dwTlsIndex)
{
    free((LPVOID *)dwTlsIndex);
    return TRUE;
}

WINBASEAPI LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex)
{
    return *(LPVOID *)dwTlsIndex;
}

WINBASEAPI BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    *(LPVOID *)dwTlsIndex = lpTlsValue;
    return TRUE;
}
#else
ALIAS("TlsAlloc", 0);
ALIAS("TlsFree", 4);
ALIAS("TlsGetValue", 4);
ALIAS("TlsSetValue", 8);
#endif

WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName)
{
    (void)lpModuleName;
    STUB("GetModuleHandleA: %s", lpModuleName);
    return NULL;
}

WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFileName, DWORD nSize)
{
    (void)hModule;
    (void)lpFileName;
    (void)nSize;
    STUB("GetModuleFileNameA: hModule=%p", hModule);
    return 0;
}

WINBASEAPI HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName)
{
    (void)lpLibFileName;
    STUB("HACK: LoadLibraryA");
    HANDLE res = (HANDLE)-1;
    TR("LoadLibraryA: res=%p lpLibFileName=%s", res, lpLibFileName);
    return res;
}

WINBASEAPI BOOL WINAPI FreeLibrary(HMODULE hLibModule)
{
    (void)hLibModule;
    STUB("HACK: FreeLibrary");
    BOOL res = TRUE;
    TR("FreeLibrary: res=%d hLibModule=%p", res, hLibModule);
    return res;
}

struct alloc {
    size_t size;
    UINT flags;
    char data[];
};

WINBASEAPI HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes)
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

WINBASEAPI HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    free((struct alloc *)hMem - 1);
    return NULL;
}

WINBASEAPI DWORD WINAPI GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR *lpFilePart)
{
    DWORD res = 0;

    size_t name_len = strlen(lpFileName);
    size_t full_len = name_len;

    // If the path isn't absolute, add the current directory
    if (lpFileName[0] != '\\' &&
            (lpFileName[0] != 'Z' || lpFileName[1] != ':')) {
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

    DB("GetFullPathNameA: '%s' = '%s' (%s)", lpFileName, lpBuffer, *lpFilePart);
    return full_len;
}

WINBASEAPI DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpDistanceToMoveHigh) {
        DIE("SetFilePointer: No support for lpDistanceToMoveHigh");
    }

    DWORD res = lseek(uFile - 1, lDistanceToMove, dwMoveMethod);
    TR("SetFilePointer: res=%ld hFile=%p lDistanceToMove=%ld dwMoveMethod=%ld",
        res, hFile, lDistanceToMove, dwMoveMethod);
    return res;
}

WINBASEAPI BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpOverlapped) {
        DIE("WriteFile: No support for lpOverlapped");
    }

    int count = write(uFile - 1, lpBuffer, nNumberOfBytesToWrite);
    BOOL res = count != -1;
    if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = count != -1 ? count : 0;
    TR("WriteFile: res=%d hFile=%p nNumberOfBytesToWrite=%ld "
        "lpNumberOfBytesWritten=%ld", res, hFile, nNumberOfBytesToWrite,
        lpNumberOfBytesWritten ? *lpNumberOfBytesWritten : 0);
    return res;
}

WINBASEAPI BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpOverlapped) {
        DIE("ReadFile: No support for lpOverlapped");
    }

    int count = read(uFile - 1, lpBuffer, nNumberOfBytesToRead);
    BOOL res = count != -1;
    if (lpNumberOfBytesRead) *lpNumberOfBytesRead = count != -1 ? count : 0;
    TR("ReadFile: res=%d hFile=%p nNumberOfBytesToRead=%ld "
        "lpNumberOfBytesRead=%ld", res, hFile, nNumberOfBytesToRead,
        lpNumberOfBytesRead ? *lpNumberOfBytesRead : 0);
    return res;
}

WINBASEAPI HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
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

    char *path = path_dup_unx_c(lpFileName);
    HANDLE res = (HANDLE)(open(path, flags, 0666) + 1);
    DB("CreateFileA: res=%p lpFileName=%s", res, path);
    free(path);
    return res;

die:
    DIE("CreateFileA: '%s' %lx %lx %p %lx %lx %p",
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
    );
}

WINBASEAPI DWORD WINAPI GetTickCount(void)
{
    STUB("GetTickCount");
    return 0;
}

WINBASEAPI BOOL WINAPI DeleteFileA(LPCSTR lpFileName)
{
    char *path = path_dup_unx_c(lpFileName);
    BOOL res = unlink(path) == 0;
    DB("DeleteFileA: res=%d lpFileName=%s", res, path);
    free(path);
    return res;
}

WINBASEAPI BOOL WINAPI MoveFileA(LPCSTR,LPCSTR);
WINBASEAPI DWORD WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,va_list*);

WINBASEAPI BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
    (void)hFile;
    (void)lpCreationTime;
    (void)lpLastAccessTime;
    (void)lpLastWriteTime;
    STUB("GetFileTime: hFile=%p", hFile);
    return FALSE;
}

WINBASEAPI BOOL WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);

WINBASEAPI DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpFileSizeHigh) {
        DIE("GetFileSize: No support for lpFileSizeHigh");
    }

    struct stat buf;
    if (fstat(uFile - 1, &buf) != 0) return INVALID_FILE_SIZE;
    DWORD res = buf.st_size;
    TR("GetFileSize: res=%ld hFile=%d", res, uFile);
    return res;
}

WINBASEAPI BOOL WINAPI SetEndOfFile(HANDLE);
WINBASEAPI BOOL WINAPI CreateDirectoryA(LPCSTR,LPSECURITY_ATTRIBUTES);
WINBASEAPI BOOL WINAPI RemoveDirectoryA(LPCSTR);
WINBASEAPI BOOL WINAPI SetStdHandle(DWORD,HANDLE);

WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetSystemTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

WINBASEAPI BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
    (void)lpSystemTime;
    (void)lpFileTime;
    STUB("SystemTimeToFileTime");
    memset(lpFileTime, 0, sizeof(*lpFileTime));
    return FALSE;
}

WINBASEAPI INT WINAPI CompareFileTime(const FILETIME*,const FILETIME*);

WINBASEAPI HGLOBAL WINAPI GlobalReAlloc(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags)
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

WINBASEAPI UINT WINAPI GlobalFlags(HGLOBAL hMem)
{
    return ((struct alloc *)hMem - 1)->flags;
}

WINBASEAPI BOOL WINAPI FileTimeToSystemTime(const FILETIME*,LPSYSTEMTIME);
WINBASEAPI HRSRC WINAPI FindResourceA(HMODULE,LPCSTR,LPCSTR);
WINBASEAPI HGLOBAL WINAPI LoadResource(HMODULE,HRSRC);
WINBASEAPI LPVOID WINAPI LockResource(HGLOBAL);
WINBASEAPI DWORD WINAPI SizeofResource(HMODULE,HRSRC);
WINBASEAPI HANDLE WINAPI CreateFileMappingA(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCSTR);
WINBASEAPI LPVOID WINAPI MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,SIZE_T);
WINBASEAPI BOOL WINAPI UnmapViewOfFile(LPCVOID);

WINBASEAPI UINT WINAPI GetSystemDirectoryA(LPSTR lpBuffer, UINT uSize)
{
    (void)lpBuffer;
    (void)uSize;
    STUB("GetSystemDirectoryA");
    return 0;
}

WINBASEAPI UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize)
{
    (void)lpBuffer;
    (void)uSize;
    STUB("GetWindowsDirectoryA");
    return 0;
}

WINBASEAPI BOOL WINAPI SetFileAttributesA(LPCSTR,DWORD);
WINBASEAPI HANDLE WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);

WINBASEAPI VOID WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetLocalTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

// winnt.h

NTSYSAPI void NTAPI RtlUnwind(void*,void*,struct _EXCEPTION_RECORD*,void*);

// consoleapi.h

WINBASEAPI BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE HandlerRoutine, BOOL Add)
{
    (void)HandlerRoutine;
    (void)Add;
    STUB("SetConsoleCtrlHandler");
    return FALSE;
}

// wincon.h

WINBASEAPI BOOL WINAPI GetConsoleScreenBufferInfo(HANDLE hConsole, LPCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo)
{
    (void)hConsole;
    (void)lpConsoleScreenBufferInfo;
    STUB("GetConsoleScreenBufferInfo: hConsole=%p", hConsole);
    return FALSE;
}

// winnls.h

WINBASEAPI INT WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
WINBASEAPI BOOL WINAPI IsValidCodePage(UINT);
WINBASEAPI UINT WINAPI GetACP(void);

// timezoneapi.h

WINBASEAPI DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION);
