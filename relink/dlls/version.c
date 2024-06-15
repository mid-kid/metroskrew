
// WINE headers, used for the sake of type-checking definitions
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winver.h"

// Local headers
#include "utils.h"

DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
    (void)lptstrFilename;
    if (lpdwHandle) *lpdwHandle = 0;
    DWORD res = 0;
    STUB("GetFileVersionInfoSizeA: res=%ld lptstrFilename='%s'",
        res, lptstrFilename);
    return res;
}

#if 0
BOOL WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    (void)lptstrFilename;
    (void)dwHandle;
    (void)dwLen;
    (void)lpData;
    BOOL res = TRUE;
    STUB("GetFileVersionInfoA: res=%d lptstrFilename='%s' dwHandle=%ld dwLen=%ld",
        res, lptstrFilename, dwHandle, dwLen);
    return res;
}

BOOL WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{
    (void)pBlock;

    *puLen = 0;
    if (strcmp(lpSubBlock, "\\") == 0) {
        *lplpBuffer = (LPVOID)&(static DWORD []){
            0, 0, 0x30000, 0x89, 0x30000, 0x89
        };
    } else if (strcmp(lpSubBlock, "\\StringFileInfo\\040904B0\\CompanyName") == 0) {
        *lplpBuffer = (LPVOID)"Freescale Semiconductor, Inc";
    } else if (strcmp(lpSubBlock, "\\StringFileInfo\\040904B0\\ProductName") == 0) {
        *lplpBuffer = (LPVOID)"CodeWarrior";
    } else if (strcmp(lpSubBlock, "\\StringFileInfo\\040904B0\\FileDescription") == 0) {
        *lplpBuffer = (LPVOID)"ARM C/C++ Compiler";
    } else if (strcmp(lpSubBlock, "\\StringFileInfo\\040904B0\\LegalCopyright") == 0) {
        *lplpBuffer = (LPVOID)"Copyright \xA9 2007";
    } else if (strcmp(lpSubBlock, "\\StringFileInfo\\040904B0\\ProductVersion") == 0) {
        *lplpBuffer = (LPVOID)"3.0 build 137";
    } else {
        DIE("VerQueryValueA: lpSubBlock='%s'", lpSubBlock);
    }

    BOOL res = TRUE;
    TR("VerQueryValueA: res=%d lpSubBlock='%s'", res, lpSubBlock);
    return res;
}
#endif
