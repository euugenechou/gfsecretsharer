#ifndef __IO_H__
#define __IO_H__

#include "galois.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

bool read_secret(FILE *infile, GF8_t *secret);

void buffer_share(FILE *outfile, GF8_t *share, uint64_t size);

void read_share(FILE *infile, GF8_t *share, uint64_t size);

void buffer_secret(FILE *outfile, GF8_t secret);

void flush_buffer(FILE *outfile);

#endif

