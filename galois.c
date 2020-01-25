#include "galois.h"
#include <stdbool.h>

GF8_t gf_add(GF8_t a, GF8_t b) {
  return a ^ b;
}

GF8_t gf_sub(GF8_t a, GF8_t b) {
  return a ^ b;
}

GF8_t gf_mul(GF8_t a, GF8_t b) {
  GF8_t res = 0;

  while (a && b) {
    if (b & 1) {
      res ^= a;
    }

    if ((a >> 7) & 1) {
      a = (a << 1) ^ IRREDUCIBLE;
    } else {
      a <<= 1;
    }

    b >>= 1;
  }

  return res;
}

GF8_t gf_div(GF8_t a, GF8_t b) {
  static bool built_table;
  static GF8_t mul_inv_table[GF_MAX][GF_MAX];

  if (!built_table) {
    built_table = true;

    for (uint16_t i = 0; i < GF_MAX; i += 1) {
      for (uint16_t j = 0; j < GF_MAX; j += 1) {
        mul_inv_table[i][gf_mul(i, j)] = j;
      }
    }
  }

  return gf_mul(a, mul_inv_table[b][1]);
}
