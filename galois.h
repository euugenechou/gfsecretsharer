#ifndef __GALOIS_H__
#define __GALOIS_H__

#include "inttypes.h"

#define GF_MAX        256
#define IRREDUCIBLE   0x11B

typedef uint8_t GF8_t;

GF8_t gf_add(GF8_t a, GF8_t b);

GF8_t gf_sub(GF8_t a, GF8_t b);

GF8_t gf_mul(GF8_t a, GF8_t b);

GF8_t gf_div(GF8_t a, GF8_t b);

#endif
