#pragma once
#include"./bit.hpp"
#include"./board.hpp"
#include"./candidates.hpp"
#include"./reverse.hpp"

uint32_t xor32(void) {
  static uint32_t y = 427307;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

std::tuple<Board, Board> simulate(Board black, Board white, size_t n = 30) {
  size_t skip = 0;
  size_t step = 0;
  while(skip < 2 && step < 2 * n) {
    Board candidates = step % 2 == 1
      ? get_candidates(black, white)
      : get_candidates(white, black);
    skip = candidates ? 0 : (skip + 1);
    size_t k = candidates && xor32() % popcount(candidates);
    for(int j = 0; j < k; j++) {
      candidates &= ~(bit_floor(candidates));
    }
    Board position = bit_floor(candidates);
    Board reverse = step % 2 == 1
      ? get_reverse(black, white, position)
      : get_reverse(white, black, position);
    black ^= step % 2 == 1 ? reverse ^ position : reverse;
    white ^= step % 2 == 1 ? reverse : reverse ^ position;
    step++;
  }
  return {black, white};
}