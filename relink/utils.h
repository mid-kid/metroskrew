#pragma once

#include <stdio.h>
#include <stdlib.h>

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

char *path_dup_unx_c(const char *path);
