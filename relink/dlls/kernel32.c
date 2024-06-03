#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
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

#ifdef TRACE
#define TR(msg, ...) printf("trace: " msg "\n", ##__VA_ARGS__)
#else
#define TR(msg, ...)
#endif

#ifndef NDEBUG
#define DB(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define STUB(msg, ...) printf("stub: " msg "\n", ##__VA_ARGS__)
#else
#define DB(msg, ...)
#define STUB(msg, ...)
#endif

#define DIE(msg, ...) { printf("die: " msg "\n", ##__VA_ARGS__);exit(1); }

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

WINBASEAPI DWORD       WINAPI GetLastError(void);
WINBASEAPI HANDLE      WINAPI GetStdHandle(DWORD);
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
    if (nBufferLength < 2) return 0;
    if (!getcwd(lpBuffer + 2, nBufferLength - 2)) return 0;

    // Convert to DOS path
    lpBuffer[0] = 'Z';
    lpBuffer[1] = ':';
    for (char *c = lpBuffer; *c; c++) if (*c == '/') *c = '\\';

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
WINBASEAPI HMODULE     WINAPI GetModuleHandleA(LPCSTR);
WINBASEAPI DWORD       WINAPI GetModuleFileNameA(HMODULE,LPSTR,DWORD);
WINBASEAPI HMODULE     WINAPI LoadLibraryA(LPCSTR);

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
WINBASEAPI DWORD       WINAPI GetFullPathNameA(LPCSTR,DWORD,LPSTR,LPSTR*);
WINBASEAPI DWORD       WINAPI SetFilePointer(HANDLE,LONG,LPLONG,DWORD);
WINBASEAPI BOOL        WINAPI WriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED);
WINBASEAPI BOOL        WINAPI ReadFile(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);

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
    DB("CreateFileA: res=%p lpFileName=%s\n", res, path);
    free(path);
    return res;

die:
    DIE("CreateFileA: '%s' %lx %lx %p %lx %lx %p",
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
    );
}

WINBASEAPI DWORD       WINAPI GetTickCount(void);

WINBASEAPI BOOL WINAPI DeleteFileA(LPCSTR lpFileName)
{
    char *path = path_dup_unx_c(lpFileName);
    BOOL res = unlink(path) == 0;
    DB("DeleteFileA: res=%d lpFileName=%s\n", res, path);
    free(path);
    return res;
}

WINBASEAPI BOOL        WINAPI MoveFileA(LPCSTR,LPCSTR);
WINBASEAPI DWORD       WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,__ms_va_list*);

WINBASEAPI BOOL WINAPI GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
    STUB("GetFileTime: hFile=%p", hFile);
    return 0;
}

WINBASEAPI BOOL        WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);

WINBASEAPI DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    uintptr_t uFile = (uintptr_t)hFile;

    if (lpFileSizeHigh) DIE("GetFileSize: Only 32 bits");

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
WINBASEAPI VOID        WINAPI GetSystemTime(LPSYSTEMTIME);
WINBASEAPI BOOL        WINAPI SystemTimeToFileTime(const SYSTEMTIME*,LPFILETIME);
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
WINBASEAPI UINT        WINAPI GetSystemDirectoryA(LPSTR,UINT);
WINBASEAPI UINT        WINAPI GetWindowsDirectoryA(LPSTR,UINT);
WINBASEAPI BOOL        WINAPI SetFileAttributesA(LPCSTR,DWORD);
WINBASEAPI HANDLE      WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);
WINBASEAPI VOID        WINAPI GetLocalTime(LPSYSTEMTIME);

// winnt.h

NTSYSAPI void    NTAPI RtlUnwind(void*,void*,struct _EXCEPTION_RECORD*,void*);

// consoleapi.h

WINBASEAPI BOOL    WINAPI SetConsoleCtrlHandler( PHANDLER_ROUTINE,BOOL);

// wincon.h

WINBASEAPI BOOL WINAPI GetConsoleScreenBufferInfo(HANDLE hConsole, LPCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo)
{
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
