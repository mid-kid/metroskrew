
// WINE headers, used for the sake of type-checking definitions
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"

// Requires widl-generated headers, not worth the effort for this file.
//#include "objbase.h"
#define interface struct
//#include "unknwn.h"
typedef interface IUnknown IUnknown;
typedef IUnknown *LPUNKNOWN;

// Local headers
#include "_utils.h"

HRESULT WINAPI CoInitialize(LPVOID pvReserved)
{
    (void)pvReserved;
    STUB("CoInitialize");
    return E_UNEXPECTED;
}

void WINAPI CoUninitialize(void);

HRESULT WINAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv)
{
    (void)rclsid;
    (void)pUnkOuter;
    (void)dwClsContext;
    (void)riid;
    (void)ppv;
    STUB("CoCreateInstance");
    return E_NOINTERFACE;
}

LPVOID WINAPI CoTaskMemAlloc(SIZE_T size);
void WINAPI CoTaskMemFree(LPVOID ptr);
