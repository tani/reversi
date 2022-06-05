#pragma once
#include<cstdint>
#include<cstddef>
#include<functional>

using Board = uint64_t;

constexpr Board board(Board a7, Board a6, Board a5, Board a4, Board a3, Board a2, Board a1, Board a0) {
  return a7 << 8 * 7 | a6 << 8 * 6 | a5 << 8 * 5 | a4 << 8 * 4
       | a3 << 8 * 3 | a2 << 8 * 2 | a1 << 8 * 1 | a0 << 8 * 0;
}

constexpr Board HorizontalMask =
  board(
    0b01111110ull,
    0b01111110ull,
    0b01111110ull,
    0b01111110ull,
    0b01111110ull,
    0b01111110ull,
    0b01111110ull,
    0b01111110ull
  );

constexpr Board VerticalMask =
  board(
    0b00000000ull,
    0b11111111ull,
    0b11111111ull,
    0b11111111ull,
    0b11111111ull,
    0b11111111ull,
    0b11111111ull,
    0b00000000ull
  );
