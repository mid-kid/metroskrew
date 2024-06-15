#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef ETRACE
#define TRACE(msg, ...) printf("trace: " msg "\n", ##__VA_ARGS__)
#else
#define TRACE(msg, ...)
#endif

#ifndef NDEBUG
#define DEBUG(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define STUB(msg, ...) printf("stub: " msg "\n", ##__VA_ARGS__)
#else
#define DEBUG(msg, ...)
#define STUB(msg, ...)
#endif

#define DIE(msg, ...) { printf("die: " msg "\n", ##__VA_ARGS__);exit(1); }

#define ALIAS(name, num) \
    __asm__(".global _" name "; .set _" name ", _" name "@" #num)

char *path_dup_unx_c(const char *path);
