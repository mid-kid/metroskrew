
// WINE headers, used for the sake of type-checking definitions
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winver.h"

// Local headers
#include "utils.h"

WINBASEAPI DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{
    (void)lptstrFilename;
    if (lpdwHandle) *lpdwHandle = 0;
    DWORD res = 0;
    STUB("GetFileVersionInfoSizeA: res=%ld lptstrFilename='%s'",
        res, lptstrFilename);
    return res;
}
