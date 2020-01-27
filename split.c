#include "galois.h"
#include "poly.h"
#include "io.h"
#include "util.h"
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THRESHOLD   5
#define SHARES      10
#define OPTIONS     "k:n:i:o:r:h"

void print_usage(char **argv) {
  fprintf(stderr,
    "SYNOPSIS\n"
    "   Generates shares using Shamir's Secret Sharing Scheme.\n"
    "   Shares are joined using the corresponding join program.\n"
    "\n"
    "USAGE\n"
    "   %s [-h] [-r seed] [-k threshold] [-n shares] [-i input] [-o output]\n"
    "\n"
    "OPTIONS\n"
    "   -h              Display program usage\n"
    "   -r seed         Specify random seed (default 1337)\n"
    "   -k threshold    Specify threshold for number of shares (default: 5)\n"
    "   -n shares       Specify number of shares to generate (default: 10)\n"
    "   -i input        Specify input (secret to split) (default: stdin)\n"
    "   -o output       Specify output (default: stdout)\n",
    argv[0]);
  return;
}

void split(FILE *infile, FILE *outfile, GF8_t k, GF8_t n) {
  GF8_t **shares = (GF8_t **)malloc(n * sizeof(GF8_t *));
  check(shares, "Failed to allocate array of shares.\n");

  GF8_t secret = 0;
  uint64_t size = 1;
  uint64_t capacity = 4096;

  for (GF8_t i = 0; i < n; i += 1) {
    shares[i] = (GF8_t *)malloc(capacity * sizeof(GF8_t));
    check(shares[i], "Failed to allocate share.\n");

    shares[i][0] = i + 1;
  }

  while (read_secret(infile, &secret)) {
    Polynomial *p = polynomial_create(k, secret);

    for (GF8_t x = 1; x < n + 1; x += 1) {
      shares[x - 1][size] = polynomial_eval(p, x);
    }

    if (++size == capacity) {
      capacity *= 2;

      for (GF8_t i = 0; i < n; i += 1) {
        shares[i] = (GF8_t *)realloc(shares[i], capacity * sizeof(GF8_t));
        check(shares[i], "Failed to reallocate share.\n");
      }
    }

    polynomial_delete(p);
  }

  for (GF8_t i = 0; i < n; i += 1) {
    buffer_share(outfile, shares[i], size);
    free(shares[i]);
  }

  flush_buffer(outfile);
  free(shares);
  return;
}

int main(int argc, char **argv) {
  int opt = 0;
  FILE *infile = stdin;
  FILE *outfile = stdout;
  uint32_t seed = time(NULL);
  uint64_t shares = SHARES;
  uint64_t threshold = THRESHOLD;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'k':
      threshold = (uint64_t)strtoull(optarg, NULL, 10);
      break;
    case 'n':
      shares = (uint64_t)strtoull(optarg, NULL, 10);
      break;
    case 'i':
      infile = fopen(optarg, "rb");
      check(infile, "Failed to open %s\n", optarg);
      break;
    case 'o':
      outfile = fopen(optarg, "wb");
      check(outfile, "Failed to open %s\n", optarg);
      break;
    case 'r':
      seed = (uint32_t)strtoul(optarg, NULL, 10);
      break;
    case 'h':
      print_usage(argv);
      return EXIT_SUCCESS;
    default:
      print_usage(argv);
      return EXIT_FAILURE;
    }
  }

  check(threshold <= shares, "Not enough shares for threshold!\n");
  check(threshold < GF_MAX, "Threshold too high for GF(256)!\n");
  check(shares < GF_MAX, "Too many shares for GF(256)!\n");

  srand(seed);
  split(infile, outfile, threshold, shares);

  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
}
