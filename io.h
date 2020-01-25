#ifndef __IO_H__
#define __IO_H__

#include "galois.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

bool read_byte(FILE *infile, uint8_t *byte);

void write_share(FILE *outfile, GF8_t *share, uint64_t size);

void read_share(FILE *infile, GF8_t *share, uint64_t size);

#endif

