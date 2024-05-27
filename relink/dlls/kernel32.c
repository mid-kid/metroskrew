#include <stdlib.h>
#include <stdio.h>

#ifdef TRACE
#define TR(msg, ...) printf("trace: " msg "\n", ##__VA_ARGS__)
#else
#define TR(msg, ...)
#endif

void ExitProcess(unsigned uExitCode)
{ TR("ExitProcess: uExitCode=%d", uExitCode);
    exit(uExitCode);
}
