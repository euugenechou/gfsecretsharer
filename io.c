#include "galois.h"
#include "io.h"
#include "util.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t byte_buf[BLOCK] = { 0 };
static uint16_t byte_cnt = 0;
static uint16_t byte_buf_end = UINT16_MAX;

bool read_byte(FILE *infile, uint8_t *byte) {
  if (!byte_cnt) {
    uint16_t bytes_read = fread(byte_buf, sizeof(uint8_t), BLOCK, infile);
    byte_buf_end = (bytes_read == BLOCK) ? byte_buf_end : bytes_read + 1;
  }

  *byte = byte_buf[byte_cnt];
  byte_cnt = (byte_cnt + 1) % BLOCK;

  return byte_cnt != byte_buf_end;
}

void write_share(FILE *outfile, GF8_t *share, uint64_t size) {
  for (uint64_t i = 0; i < size; i += 1) {
    fprintf(outfile, "%02x", share[i]);
  }

  fprintf(outfile, "\n");
  return;
}

void read_share(FILE *infile, GF8_t *share, uint64_t size) {
  uint8_t hex[3] = { 0 };
  uint64_t share_ind = 0;

  for (uint64_t i = 0; i < size; i += 1) {
    if (!byte_cnt) {
      uint16_t bytes_read = fread(byte_buf, sizeof(uint8_t), BLOCK, infile);
      byte_buf_end = (bytes_read == BLOCK) ? byte_buf_end : bytes_read + 1;
    }

    if (i & 1) {
      hex[1] = byte_buf[byte_cnt];
      share[share_ind++] = strtol((char *)hex, NULL, 16);
    } else {
      hex[0] = byte_buf[byte_cnt];
    }

    byte_cnt = (byte_cnt + 1) % BLOCK;
  }

  byte_cnt = (byte_cnt + 1) % BLOCK;
  return;
}
