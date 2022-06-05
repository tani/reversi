#pragma once
#include<tuple>

uint32_t xor32(void) {
  static uint32_t y = 427307;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

constexpr std::tuple<uint32_t, uint32_t> rand(uint32_t y) {
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return {y ^ (y << 5), y};
}
