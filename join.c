#include "galois.h"
#include "io.h"
#include "poly.h"
#include "util.h"
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE      20
#define SHARES    10
#define OPTIONS   "n:k:i:o:h"

void print_usage(char **argv) {
  fprintf(stderr,
    "SYNOPSIS\n"
    "   Interpolates shares for Shamir's Secret Sharing Scheme.\n"
    "   Shares are generated using the corresponding split program.\n"
    "\n"
    "USAGE\n"
    "   %s [-h] [-n shares] [-i input] [-o output]\n"
    "\n"
    "OPTIONS\n"
    "   -h            Display program usage\n"
    "   -n shares     Specify number of shares to join (default: 10)\n"
    "   -k size       Specify size of shares (in bytes) to join.\n"
    "   -i input      Specify input (default: stdin)\n"
    "   -o output     Specify output (default: stdout)\n",
    argv[0]);
  return;
}

void join(FILE *infile, FILE *outfile, GF8_t n, uint64_t size) {
  GF8_t **shares = (GF8_t **)malloc(n * sizeof(GF8_t *));
  check(shares, "Failed to allocate array of shares.\n");

  for (GF8_t i = 0; i < n; i += 1) {
    shares[i] = (GF8_t *)malloc((size >> 1) * sizeof(GF8_t));
    check(shares[i], "Failed to allocate share.\n");

    read_share(infile, shares[i], size);
  }

  GF8_t *xs = (GF8_t *)malloc(n * sizeof(GF8_t));
  GF8_t *ys = (GF8_t *)malloc(n * sizeof(GF8_t));
  check(xs && ys, "Failed to allocate coordinate array(s).\n");

  uint8_t secret[BLOCK];
  uint16_t byte = 0;

  for (uint64_t i = 1; i < (size >> 1); i += 1) {
    for (GF8_t j = 0; j < n; j += 1) {
      xs[j] = shares[j][0];
      ys[j] = shares[j][i];
    }

    secret[byte++] = polynomial_interpolate(xs, ys, n);

    if (byte == BLOCK) {
      fwrite(secret, sizeof(uint8_t), BLOCK, outfile);
      byte = 0;
    }
  }

  if (byte) {
    fwrite(secret, sizeof(uint8_t), byte, outfile);
  }

  free(xs);
  free(ys);
  return;
}

int main(int argc, char **argv) {
  int opt = 0;
  FILE *infile = stdin;
  FILE *outfile = stdout;
  uint64_t size = SIZE;
  uint64_t shares = SHARES;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'k':
      size = (uint64_t)strtoull(optarg, NULL, 10);
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
    case 'h':
      print_usage(argv);
      return EXIT_SUCCESS;
    default:
      print_usage(argv);
      return EXIT_FAILURE;
    }
  }

  join(infile, outfile, shares, size);

  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
}