
// WINE headers, used for the sake of type-checking definitions
#define WINADVAPI
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winreg.h"

// Local headers
#include "_utils.h"

LSTATUS WINAPI RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    (void)hKey;
    (void)lpSubKey;
    (void)ulOptions;
    (void)samDesired;
    (void)phkResult;
    STUB("RegOpenKeyExA: lpSubKey='%s'", lpSubKey);
    return ERROR_FILE_NOT_FOUND;
}

LSTATUS   WINAPI RegQueryValueExA(HKEY,LPCSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
LSTATUS   WINAPI RegCloseKey(HKEY);
