#include "galois.h"
#include "io.h"
#include "util.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t total_read = 0;
uint64_t total_written = 0;

static uint8_t in_buf[BLOCK];
static uint16_t in_cnt = 0;
static uint16_t in_end = UINT16_MAX;

static uint8_t out_buf[BLOCK];
static uint16_t out_cnt = 0;

static void read_buffer(FILE *infile) {
  if (!in_cnt) {
    uint16_t bytes_read = fread(in_buf, sizeof(uint8_t), BLOCK, infile);
    in_end = (bytes_read == BLOCK) ? in_end : bytes_read + 1;
    total_read += bytes_read;
  }

  return;
}

static void write_buffer(FILE *outfile) {
  if (out_cnt == BLOCK) {
    fwrite(out_buf, sizeof(uint8_t), BLOCK, outfile);
    out_cnt = 0;
    total_written += BLOCK;
  }

  return;
}

bool read_secret(FILE *infile, GF8_t *secret) {
  read_buffer(infile);
  *secret = in_buf[in_cnt];
  in_cnt = (in_cnt + 1) % BLOCK;
  return in_cnt != in_end;
}

void buffer_share(FILE *outfile, GF8_t *share, uint64_t size) {
  const char *hex = "0123456789abcdef";

  for (uint64_t i = 0; i < size; i += 1) {
    for (uint64_t j = 0; j < 2; j += 1) {
      uint8_t shift = (j & 1) ? 0 : 4;
      out_buf[out_cnt++] = hex[(share[i] >> shift) & 0xF];
      write_buffer(outfile);
    }
  }

  out_buf[out_cnt++] = '\n';
  write_buffer(outfile);
  return;
}

void read_share(FILE *infile, GF8_t *share, uint64_t size) {
  uint64_t share_ind = 0;

  for (uint64_t i = 0; i < size; i += 1) {
    read_buffer(infile);

    if (i & 1) {
      if (in_buf[in_cnt] > 57) {
        share[share_ind++] |= (in_buf[in_cnt] - 55) & 0xF;
      } else {
        share[share_ind++] |= (in_buf[in_cnt] - 48) & 0xF;
      }
    } else {
      if (in_buf[in_cnt] > 57) {
        share[share_ind] = ((in_buf[in_cnt] - 55) & 0xF) << 4;
      } else {
        share[share_ind] = ((in_buf[in_cnt] - 48) & 0xF) << 4;
      }
    }

    in_cnt = (in_cnt + 1) % BLOCK;
  }

  in_cnt = (in_cnt + 1) % BLOCK;
  return;
}

void buffer_secret(FILE *outfile, GF8_t secret) {
  out_buf[out_cnt++] = secret;
  write_buffer(outfile);
  return;
}

void flush_buffer(FILE *outfile) {
  if (out_cnt) {
    fwrite(out_buf, sizeof(uint8_t), out_cnt, outfile);
    total_written += out_cnt;
  }

  return;
}
