#pragma once
#include<cstdint>

using Board = uint64_t;

Board HorizontalMask = (
    0b01111110ull << 8 * 7
  | 0b01111110ull << 8 * 6
  | 0b01111110ull << 8 * 5
  | 0b01111110ull << 8 * 4
  | 0b01111110ull << 8 * 3
  | 0b01111110ull << 8 * 2
  | 0b01111110ull << 8 * 1
  | 0b01111110ull << 8 * 0
);
Board VerticalMask = (
    0b00000000ull << 8 * 7
  | 0b11111111ull << 8 * 6
  | 0b11111111ull << 8 * 5
  | 0b11111111ull << 8 * 4
  | 0b11111111ull << 8 * 3
  | 0b11111111ull << 8 * 2
  | 0b11111111ull << 8 * 1
  | 0b00000000ull << 8 * 0
);
