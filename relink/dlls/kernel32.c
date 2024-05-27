#include <stdlib.h>
#include <stdio.h>

// WINE headers, used for the sake of type-checking definitions
#include "windef.h"
#include "winbase.h"

#ifdef TRACE
#define TR(msg, ...) printf("trace: " msg "\n", ##__VA_ARGS__)
#else
#define TR(msg, ...)
#endif

WINBASEAPI VOID DECLSPEC_NORETURN WINAPI ExitProcess(DWORD uExitCode)
{ TR("ExitProcess: uExitCode=%lu", uExitCode);
    exit(uExitCode);
}
