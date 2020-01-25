#include "util.h"
#include "galois.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void print_share(GF8_t *share, uint64_t size) {
  printf("Share: (");

  for (uint64_t i = 0; i < size; i += 1) {
    printf("%" PRIu8, share[i]);

    if (i != size - 1){
      printf(", ");
    }
  }

  printf(")\n");
  return;
}

void check(bool cond, char *fmt, ...) {
  if (!cond) {
    va_list arg;
    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    exit(EXIT_FAILURE);
  }

  return;
}

