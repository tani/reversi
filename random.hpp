#pragma once

uint32_t xor32(void) {
  static uint32_t y = 427307;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

