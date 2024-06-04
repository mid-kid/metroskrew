#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// WINE headers, used for the sake of type-checking definitions
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

WINBASEAPI BOOL        WINAPI IsBadReadPtr(LPCVOID,UINT_PTR);

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

WINBASEAPI DWORD WINAPI GetLastError(void)
{
    DWORD res = errno;
    TR("GetLastError: res=%ld", res);
    return res;
}

WINBASEAPI HANDLE WINAPI GetStdHandle(DWORD nStdHandle)
{
    DWORD num = -nStdHandle - 10;
    if (num > 2) return INVALID_HANDLE_VALUE;
    HANDLE res = (HANDLE)(num + 1);
    TR("GetStdHandle: res=%ld nStdHandle=%ld", (DWORD)res, nStdHandle);
    return res;
}

WINBASEAPI void        WINAPI InitializeCriticalSection(CRITICAL_SECTION *lpCrit);
WINBASEAPI void        WINAPI DeleteCriticalSection(CRITICAL_SECTION *lpCrit);
WINBASEAPI void        WINAPI EnterCriticalSection(CRITICAL_SECTION *lpCrit);
WINBASEAPI void        WINAPI LeaveCriticalSection(CRITICAL_SECTION *lpCrit);
WINBASEAPI HANDLE      WINAPI FindFirstFileA(LPCSTR,LPWIN32_FIND_DATAA);

DWORD GetFileAttributes_do(char *path)
{
    struct stat buf;
    if (stat(path, &buf) != 0) return INVALID_FILE_ATTRIBUTES;

    DWORD res = 0;
    res |= (buf.st_mode & S_IFMT) == S_IFDIR ?
        FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
    return res;
}

WINBASEAPI DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName)
{
    char *path = path_dup_unx_c(lpFileName);
    STUB("GetFileAttributes: only presence and directory: %s", path);
    DWORD res = GetFileAttributes_do(path);
    DB("GetFileAttributesA: res=%lx lpFileName=%s", res, path);
    free(path);
    return res;
}

WINBASEAPI BOOL        WINAPI FindNextFileA(HANDLE,LPWIN32_FIND_DATAA);
WINBASEAPI BOOL        WINAPI FindClose(HANDLE);
WINBASEAPI LPSTR       WINAPI GetCommandLineA(void);
WINBASEAPI LPSTR       WINAPI GetEnvironmentStringsA(void);
WINBASEAPI BOOL        WINAPI FreeEnvironmentStringsA(LPSTR);

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

WINBASEAPI BOOL        WINAPI CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
WINBASEAPI DWORD       WINAPI WaitForSingleObject(HANDLE,DWORD);
WINBASEAPI BOOL        WINAPI GetExitCodeProcess(HANDLE,LPDWORD);

WINBASEAPI BOOL WINAPI CloseHandle(HANDLE hObject)
{
    uintptr_t uObject = (uintptr_t)hObject;
    BOOL res = TRUE;
    if (uObject > 3) res = close(uObject - 1) == 0;
    TR("CloseHandle: res=%d hObject=%p", res, hObject);
    return res;
}

WINBASEAPI DWORD       WINAPI TlsAlloc(void);
WINBASEAPI BOOL        WINAPI TlsFree(DWORD);
WINBASEAPI LPVOID      WINAPI TlsGetValue(DWORD);
WINBASEAPI BOOL        WINAPI TlsSetValue(DWORD,LPVOID);

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

WINBASEAPI HGLOBAL     WINAPI GlobalAlloc(UINT,SIZE_T) __WINE_ALLOC_SIZE(2) __WINE_DEALLOC(GlobalFree) __WINE_MALLOC;
WINBASEAPI HGLOBAL     WINAPI GlobalFree(HGLOBAL);

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
    if (!part) part = lpBuffer;
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
    TR("WriteFile: res=%d hFile=%p nNumberOfBytesToWrite=%ld"
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
    TR("ReadFile: res=%d hFile=%p nNumberOfBytesToRead=%ld"
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

WINBASEAPI BOOL        WINAPI MoveFileA(LPCSTR,LPCSTR);
WINBASEAPI DWORD       WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,__ms_va_list*);

WINBASEAPI BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
    (void)hFile;
    (void)lpCreationTime;
    (void)lpLastAccessTime;
    (void)lpLastWriteTime;
    STUB("GetFileTime: hFile=%p", hFile);
    return FALSE;
}

WINBASEAPI BOOL        WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);

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

WINBASEAPI BOOL        WINAPI SetEndOfFile(HANDLE);
WINBASEAPI BOOL        WINAPI CreateDirectoryA(LPCSTR,LPSECURITY_ATTRIBUTES);
WINBASEAPI BOOL        WINAPI RemoveDirectoryA(LPCSTR);
WINBASEAPI BOOL        WINAPI SetStdHandle(DWORD,HANDLE);

WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetSystemTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

WINBASEAPI BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
    (void)lpSystemTime;
    (void)lpFileTime;
    return FALSE;
}

WINBASEAPI INT         WINAPI CompareFileTime(const FILETIME*,const FILETIME*);
WINBASEAPI HGLOBAL     WINAPI GlobalReAlloc(HGLOBAL,SIZE_T,UINT) __WINE_ALLOC_SIZE(2) __WINE_DEALLOC(GlobalFree);
WINBASEAPI UINT        WINAPI GlobalFlags(HGLOBAL);
WINBASEAPI BOOL        WINAPI FileTimeToSystemTime(const FILETIME*,LPSYSTEMTIME);
WINBASEAPI HRSRC       WINAPI FindResourceA(HMODULE,LPCSTR,LPCSTR);
WINBASEAPI HGLOBAL     WINAPI LoadResource(HMODULE,HRSRC);
WINBASEAPI LPVOID      WINAPI LockResource(HGLOBAL);
WINBASEAPI DWORD       WINAPI SizeofResource(HMODULE,HRSRC);
WINBASEAPI HANDLE      WINAPI CreateFileMappingA(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCSTR);
WINBASEAPI LPVOID      WINAPI MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,SIZE_T);
WINBASEAPI BOOL        WINAPI UnmapViewOfFile(LPCVOID);

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

WINBASEAPI BOOL        WINAPI SetFileAttributesA(LPCSTR,DWORD);
WINBASEAPI HANDLE      WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);

WINBASEAPI VOID WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    STUB("GetLocalTime");
    memset(lpSystemTime, 0, sizeof(*lpSystemTime));
}

// winnt.h

NTSYSAPI void    NTAPI RtlUnwind(void*,void*,struct _EXCEPTION_RECORD*,void*);

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

WINBASEAPI INT         WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
WINBASEAPI BOOL        WINAPI IsValidCodePage(UINT);
WINBASEAPI UINT        WINAPI GetACP(void);

// timezoneapi.h

WINBASEAPI DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION);
