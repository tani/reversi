#pragma once
#include"./board.hpp"
#include"./bit.hpp"
#include"./test.hpp"
#include"./string.hpp"

#if defined(__AVX2__)
#  if defined(__MSC_VER)
#    include<intrin.h>
#  else
#    include<x86intrin.h>
#  endif
Board get_candidates_simd256(Board black, Board white) {
  alignas(32) Board mask[4] = {
    white & HorizontalMask, white & HorizontalMask & VerticalMask,
    white & VerticalMask, white & HorizontalMask & VerticalMask
  };
  alignas(32) Board diff[4] = { 1, 7, 8, 9 };
  alignas(32) Board pattern[4] = { black, black, black, black };
  __m256i mm_mask = _mm256_load_si256(reinterpret_cast<__m256i*>(mask));
  __m256i mm_diff = _mm256_load_si256(reinterpret_cast<__m256i*>(diff));
  __m256i mm_pattern1 = _mm256_load_si256(reinterpret_cast<__m256i*>(pattern));
  __m256i mm_pattern3 = _mm256_load_si256(reinterpret_cast<__m256i*>(pattern));
  mm_pattern1 = _mm256_and_si256(mm_mask, _mm256_sllv_epi64(mm_pattern1, mm_diff));
  mm_pattern3 = _mm256_and_si256(mm_mask, _mm256_srlv_epi64(mm_pattern3, mm_diff));
  for(size_t i; i < 5; i++) {
    mm_pattern1 = _mm256_or_si256(mm_pattern1, _mm256_and_si256(mm_mask, _mm256_sllv_epi64(mm_pattern1, mm_diff)));
    mm_pattern3 = _mm256_or_si256(mm_pattern3, _mm256_and_si256(mm_mask, _mm256_srlv_epi64(mm_pattern3, mm_diff)));
  }
  mm_pattern1 = _mm256_sllv_epi64(mm_pattern1, mm_diff);
  mm_pattern3 = _mm256_srlv_epi64(mm_pattern3, mm_diff);
  _mm256_store_si256(reinterpret_cast<__m256i*>(pattern), _mm256_or_si256(mm_pattern1, mm_pattern3));
  return (pattern[0] | pattern[1] | pattern[2] | pattern[3]) & ~ (black | white);
}
Board get_candidates(Board black, Board white) {
  return get_candidates_simd256(black, white);
}
#else
constexpr Board get_candidates_nosimd(Board black, Board white) {
  Board result1 = 0, result2 = 0, pattern1 = 0, pattern2 = 0;
  const Board mask[4] = {
    white & HorizontalMask, white & HorizontalMask & VerticalMask,
    white & VerticalMask, white & VerticalMask & HorizontalMask
  };
  const size_t diff[4] = { 1, 7, 8, 9 };
  for(int i = 0; i < 4; i++) {
    pattern1 = mask[i] & (black << diff[i]);
    pattern2 = mask[i] & (black >> diff[i]);
    for(int j = 0; j < 5; j++) {
      pattern1 |= mask[i] & (pattern1 << diff[i]);
      pattern2 |= mask[i] & (pattern2 >> diff[i]);
    }
    result1 |= pattern1 << diff[i];
    result2 |= pattern2 >> diff[i];
  }
  
  return (result1 | result2) & ~(black | white);
}
TEST_BEGIN(get_candidates)
constexpr Board black =
  board(
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00010000ull,
    0b00001000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull
  );
constexpr Board white =
  board(
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00001000ull,
    0b00010000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull
  );
constexpr Board candidates =
  board(
    0b00000000ull,
    0b00000000ull,
    0b00001000ull,
    0b00000100ull,
    0b00100000ull,
    0b00010000ull,
    0b00000000ull,
    0b00000000ull
  );
static_assert(get_candidates_nosimd(black, white) == candidates ,"");
TEST_END(get_candidates)

Board get_candidates(Board black, Board white) {
  return get_candidates_nosimd(black, white);
}
#endif
