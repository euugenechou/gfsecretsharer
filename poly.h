#ifndef __POLY_H__
#define __POLY_H__

#include "galois.h"
#include <inttypes.h>

typedef struct Polynomial {
  GF8_t *coeffs;
  GF8_t degree;
} Polynomial;

Polynomial *polynomial_create(GF8_t degree, GF8_t secret);

void polynomial_delete(Polynomial *p);

GF8_t polynomial_eval(Polynomial *p, GF8_t x);

GF8_t polynomial_interpolate(GF8_t *xs, GF8_t *ys, GF8_t shares);

void polynomial_print(Polynomial *p);

#endif
