/*
 * shake64.c
 *
 * Copyright 2013 Johannes Baagøe <baagoe@baagoe.org>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" and Johannes Baagøe DISCLAIMS ALL
 * WARRANTIES with regard to this software including all implied
 * warranties of merchantability and fitness. In no event shall
 * Johannes Baagøe be liable for any special, direct, indirect, or
 * consequential damages or any damages whatsoever resulting from
 * loss of use, data or profits, whether in an action of contract,
 * negligence or other tortious action, arising out of or in connection
 * with the use or performance of this software.
 */

#include "shake64.h"

#ifdef NO_RISK_OF_HASHDOS
#define HASHDOS_GUARD 0
#else
#define HASHDOS_GUARD seed
#endif

static inline uint64_t rotl64(
  uint64_t n,
  int r
) {
  return n << r | n >> (64 - r);
}

static inline uint64_t mix64(
  uint64_t h,
  uint64_t k
) {
  k *= 0x6e615aece563607dULL;  // 7 * 640687 * 1773484309013
  k = rotl64(k, 14);
  k *= 0xbcc369a20fa0b113ULL;  // 23 * 2903 * 203714769494179

  h ^= k;

  h = rotl64(h, 5);
  h *= 9;
  return h;
}

static inline uint64_t scramble64(
  uint64_t n
) {
  n ^= n >> 32;
  n *= 0x0dcfe0adfa6866c1ULL;
  if (n & 1) {                   // n is odd; n + 1 is even
    if (~n) {                    // make an exception for -1
      n *= (n + 1) / 2;
    } else {                     // -1: multiplication would overflow
      n = 0x8000000000000000ULL; // but the result is known
    }
  } else {                       // n is even, we can divide by 2
    n = (n + 1) * (n / 2);
  }
  n ^= n >> 32;
  return n;
}

uint64_t shake64(
  const void *message,
  size_t len,
  uint64_t seed,
  uint64_t *hash1
) {

  const int words_per_block = 12;
  const int bytes_per_word = sizeof (uint64_t);
  const int bytes_per_block = bytes_per_word * words_per_block;

  uint64_t s0 = 0x243f6a8885a308d3ULL ^ seed ^ HASHDOS_GUARD;
  uint64_t s1 = 0x13198a2e03707344ULL;
  uint64_t s2 = 0xa4093822299f31d0ULL;
  uint64_t s3 = 0x082efa98ec4e6c89ULL;
  uint64_t s4 = 0x452821e638d01377ULL;
  uint64_t s5 = 0xbe5466cf34e90c6cULL;
  uint64_t s6 = 0xc0ac29b7c97c50ddULL;
  uint64_t s7 = 0x3f84d5b5b5470917ULL;
  uint64_t s8 = 0x9216d5d98979fb1bULL;
  uint64_t s9 = 0xd1310ba698dfb5acULL;
  uint64_t sa = 0x2ffd72dbd01adfb7ULL;
  uint64_t sb = 0xb8e1afed6a267e96ULL;

  uint64_t *data = (uint64_t *) message;

  const uint64_t *blocks_limit = data
    + len / bytes_per_block * words_per_block;
  const uint64_t *words_limit = data + len / bytes_per_word;

  while (data < blocks_limit) {
    s0 += *data++; s2 ^= s1; s3 = rotl64(s3, 17); s4 ^= s7; s5 += s6;
    s1 += *data++; s3 ^= s2; s4 = rotl64(s4, 17); s5 ^= s8; s6 += s7;
    s2 += *data++; s4 ^= s3; s5 = rotl64(s5, 17); s6 ^= s9; s7 += s8;
    s3 += *data++; s5 ^= s4; s6 = rotl64(s6, 17); s7 ^= sa; s8 += s9;
    s4 += *data++; s6 ^= s5; s7 = rotl64(s7, 17); s8 ^= sb; s9 += sa;
    s5 += *data++; s7 ^= s6; s8 = rotl64(s8, 17); s9 ^= s0; sa += sb;
    s6 += *data++; s8 ^= s7; s9 = rotl64(s9, 17); sa ^= s1; sb += s0;
    s7 += *data++; s9 ^= s8; sa = rotl64(sa, 17); sb ^= s2; s0 += s1;
    s8 += *data++; sa ^= s9; sb = rotl64(sb, 17); s0 ^= s3; s1 += s2;
    s9 += *data++; sb ^= sa; s0 = rotl64(s0, 17); s1 ^= s4; s2 += s3;
    sa += *data++; s0 ^= sb; s1 = rotl64(s1, 17); s2 ^= s5; s3 += s4;
    sb += *data++; s1 ^= s0; s2 = rotl64(s2, 17); s3 ^= s6; s4 += s5;
  }

  s0 = mix64(s0, HASHDOS_GUARD + len);

  if (len >= bytes_per_block) { // condense the state into s0
    s0 = mix64(s0, s6);
    s1 = mix64(s1, s7);
    s2 = mix64(s2, s8);
    s3 = mix64(s3, s9);
    s4 = mix64(s4, sa);
    s5 = mix64(s5, sb);

    s0 = mix64(s0, s4);
    s1 = mix64(s1, s5);
    s2 = mix64(s2, s6);

    s0 = mix64(s0, s1);
    s2 = mix64(s2, s3);

    s0 = mix64(s0, s2);
  }

  while (data < words_limit) {
    s0 = mix64(s0 ^ HASHDOS_GUARD, *data++);
  }

  switch (len & (bytes_per_word - 1)) {
  case 1:
    s0 = mix64(s0, *data & 0x00000000000000ffULL);
    break;
  case 2:
    s0 = mix64(s0, *data & 0x000000000000ffffULL);
    break;
  case 3:
    s0 = mix64(s0, *data & 0x0000000000ffffffULL);
    break;
  case 4:
    s0 = mix64(s0, *data & 0x00000000ffffffffULL);
    break;
  case 5:
    s0 = mix64(s0, *data & 0x000000ffffffffffULL);
    break;
  case 6:
    s0 = mix64(s0, *data & 0x0000ffffffffffffULL);
    break;
  case 7:
    s0 = mix64(s0, *data & 0x00ffffffffffffffULL);
    break;
  }

  if (hash1 != NULL) {
    *hash1 = scramble64(mix64(s0, s2));
  }

  return scramble64(s0);
}
