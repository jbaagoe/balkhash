/*
 * shake32.c
 *
 * Copyright 2013 Johannes Baagøe <baagoe@baagoe.org>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" and Johannes Baagøe disclaims all
 * warranties with regard to this software including all implied
 * warranties of merchantability and fitness. In no event shall
 * Johannes Baagøe be liable for any special, direct, indirect, or
 * consequential damages or any damages whatsoever resulting from
 * loss of use, data or profits, whether in an action of contract,
 * negligence or other tortious action, arising out of or in connection
 * with the use or performance of this software.
 */

#include "shake32.h"

#ifdef NO_RISK_OF_HASHDOS
#define HASHDOS_GUARD (0)
#else
#define HASHDOS_GUARD seed
#endif

static inline uint32_t rotl32(
  uint32_t n,
  int r
) {
  return n << r | n >> (32 - r);
}

static inline uint32_t combine32(
  uint32_t n,
  uint32_t k
) {
  n += k * 0x6ff9766f;
  n = rotl32(n, 14);
  n = n * 0x525bd4ab;
  return n;
}

static inline uint32_t mix32(
  uint32_t n,
  uint32_t k
) {
  n += k * 0x5e6aca55;
  n = rotl32(n, 16);
  n *= 0x53987a97;
  n = rotl32(n, 15);
  n *= 0xe3522d45;
  return n;
}

static inline uint32_t scramble32(
  uint32_t h
) {
  h ^= h >> 16;
  h *= 0xeb1bcb4b;
  h ^= h >> 15;
  h *= 0x5b15a697;
  h ^= h >> 16;
  return h;
}

uint32_t shake32(
  const void *message,
  size_t len,
  uint32_t seed
) {
  const int bytes_per_word = sizeof (uint32_t);
  const int words_per_block = 4;
  const int bytes_per_block = bytes_per_word * words_per_block;

  uint32_t s0 = 0x243f6a88 +  seed - HASHDOS_GUARD;
  uint32_t s1 = 0x85a308d3;
  uint32_t s2 = 0x13198a2e;
  uint32_t s3 = 0x03707344;

  uint32_t *data = (uint32_t *) message;

  const uint32_t *blocks_limit = data
    + len / bytes_per_block * words_per_block;
  const uint32_t *words_limit = data + len / bytes_per_word;

  while (data < blocks_limit) {
    s0 = combine32(s0 + HASHDOS_GUARD, *data++);
    s1 = combine32(s1 + HASHDOS_GUARD, *data++);
    s2 = combine32(s2 + HASHDOS_GUARD, *data++);
    s3 = combine32(s3 + HASHDOS_GUARD, *data++);
  }

  s0 = combine32(s0, len);

  if (len >= bytes_per_block) { // condense the state into s0
    s0 = combine32(s0, s1);
    s2 = combine32(s2, s3);

    s0 = combine32(s0, s2);
  }

  while (data < words_limit) {
    s0 = mix32(s0 + HASHDOS_GUARD, *data++);
  }

  switch (len & (bytes_per_word - 1)) {
  case 1:
    s0 = combine32(s0, *data & 0x000000ff);
    break;
  case 2:
    s0 = combine32(s0, *data & 0x0000ffff);
    break;
  case 3:
    s0 = combine32(s0, *data & 0x00ffffff);
    break;
  }

  return scramble32(s0);
}
