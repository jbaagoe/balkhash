/*
 * stir.c
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

#include "stir.h"

uint32_t stir(
  const void *key,
  size_t length,
  uint32_t hash
) {
  uint8_t *kp = (uint8_t *) key;
  uint8_t *end = (uint8_t *) key + length;

  while (kp < end) {
    hash *= 0xab64a9b3;
    hash += *kp++;
  }

  return hash;
}
