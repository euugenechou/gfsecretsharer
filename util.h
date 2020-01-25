#ifndef __UTIL_H__
#define __UTIL_H__

#include "galois.h"
#include <stdarg.h>
#include <stdbool.h>

#define BLOCK 4096

void print_share(GF8_t *share, uint64_t size);

void check(bool cond, char *fmt, ...);

#endif
