#include "galois.h"
#include "io.h"
#include "poly.h"
#include "util.h"
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE      20
#define SHARES    10
#define OPTIONS   "hvn:k:i:o:"

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
    "   -h            Display program usage.\n"
    "   -v            Print out size of reconstructed secret.\n"
    "   -n shares     Specify number of shares to join (default: 10).\n"
    "   -k size       Specify size of shares (in bytes) to join.\n"
    "   -i input      Specify input (default: stdin).\n"
    "   -o output     Specify output (default: stdout).\n",
    argv[0]);
  return;
}

uint64_t join(FILE *infile, FILE *outfile, GF8_t n, uint64_t size) {
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

  for (uint64_t i = 1; i < (size >> 1); i += 1) {
    for (GF8_t j = 0; j < n; j += 1) {
      xs[j] = shares[j][0];
      ys[j] = shares[j][i];
    }

    buffer_secret(outfile, polynomial_interpolate(xs, ys, n));
  }

  flush_buffer(outfile);
  free(xs);
  free(ys);

  return (size >> 1) - 1;
}

int main(int argc, char **argv) {
  int opt = 0;
  bool verbose = false;
  FILE *infile = stdin;
  FILE *outfile = stdout;
  uint64_t size = SIZE;
  uint64_t shares = SHARES;

  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'v':
      verbose = true;
      break;
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

  uint64_t secret_size = join(infile, outfile, shares, size);

  if (verbose) {
    printf("Reconstructed secret: %" PRIu64 " bytes.\n", secret_size);
  }

  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
}
