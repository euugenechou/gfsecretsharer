#include "galois.h"
#include "poly.h"
#include "util.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

Polynomial *polynomial_create(GF8_t threshold, GF8_t secret) {
  Polynomial *p = (Polynomial *)malloc(sizeof(Polynomial));
  check(p, "Failed to create polynomial.\n");

  p->degree = threshold;
  p->coeffs = (GF8_t *)malloc(p->degree * sizeof(GF8_t));
  check(p->coeffs, "Failed to create coefficient array.\n");

  for (GF8_t i = 0; i < p->degree; i += 1) {
    if (!i) {
      p->coeffs[i] = secret;
    } else {
      p->coeffs[i] = rand() & 0xFF;
    }
  }

  return p;
}

void polynomial_delete(Polynomial *p) {
  free(p->coeffs);
  p->coeffs = NULL;
  free(p);
  return;
}

GF8_t polynomial_eval(Polynomial *p, GF8_t x) {
  GF8_t y = 0;

  for (uint64_t i = 0; i < p->degree; i += 1) {
    y = gf_add(gf_mul(y, x), p->coeffs[p->degree - i - 1]);
  }

  return y;
}

GF8_t polynomial_interpolate(GF8_t *xs, GF8_t *ys, GF8_t shares) {
  GF8_t secret = 0;

  for (uint64_t i = 0; i < shares; i += 1) {
    GF8_t term = 1;

    for (uint64_t j = 0; j < shares; j += 1) {
      if (j != i) {
        term = gf_mul(term, gf_div(xs[j], gf_sub(xs[j], xs[i])));
      }
    }

    secret = gf_add(secret, gf_mul(ys[i], term));
  }

  return secret;
}

void polynomial_print(Polynomial *p) {
  for (uint64_t i = 0; i < p->degree; i += 1) {
    printf("%" PRIu8, p->coeffs[i]);

    if (i) {
      printf("x^%" PRIu64, i);
    }

    if (i != p->degree - 1) {
      printf(" + ");
    }
  }

  printf("\n");
  return;
}
