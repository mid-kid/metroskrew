#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "_utils.h"

char *patch_getenv(const char *name)
{
    char *res = NULL;
    if (strcmp(name, "MW_CYGDRIVE_PREFIX") == 0) {
        res = NULL;
    } else if (strcmp(name, "MW_CYGWIN_ROOT") == 0) {
        res = NULL;
    } else if (strcmp(name, "PATH") == 0) {
        res = NULL;
    } else if (strcmp(name, "LM_LICENSE_FILE") == 0) {
        res = NULL;
    } else if (strcmp(name, "MWCARMIncludes") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWCIncludes") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWARMLibraries") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWLibraries") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWARMLibraryFiles") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWLibraryFiles") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWAsmARMIncludes") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWAsmIncludes") == 0) {
        res = getenv(name);
    } else if (strcmp(name, "MWASMARMINCLUDES") == 0) {
        // Used by mwasmarm >=1.0-26
        res = getenv(name);
    } else if (strcmp(name, "MWASMINCLUDES") == 0) {
        // Used by mwasmarm >=1.0-26
        res = getenv(name);
    } else if (strcmp(name, "TEMP") == 0) {
        res = NULL;
    } else if (strcmp(name, "TMP") == 0) {
        res = NULL;
    } else if (strcmp(name, "TMPDIR") == 0) {
        res = NULL;
    } else {
        printf("die: getenv: name='%s'\n", name);
        exit(EXIT_FAILURE);
    }
    TRACE("getenv: '%s' = '%s'", name, res);
    return res;
}
