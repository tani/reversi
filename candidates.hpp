#pragma once
#include<valarray>
#include<cstdint>
#include"./board.hpp"

#if defined(__MSC_VER)
#  include<intrin.h>
#else
#  include<x86intrin.h>
#endif

Board get_candidates_nosimd(Board black, Board white) {
  std::valarray<Board> mask = {
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
  };

  std::valarray<Board> diff = { 1, 7, 8, 9 };
  std::valarray<Board> pattern1 = { black, black, black, black };
  std::valarray<Board> pattern2 = { black, black, black, black };

  pattern1 = mask & (pattern1 << diff);
  pattern2 = mask & (pattern2 >> diff);
  for(size_t i = 0; i < 5; i++) {
    pattern1 |= mask & (pattern1 << diff);
    pattern2 |= mask & (pattern2 >> diff);
  }
  pattern1 <<= diff; 
  pattern2 >>= diff; 

  Board candidates =
    pattern1[0] | pattern1[1] | pattern1[2] | pattern1[3] |
    pattern2[0] | pattern2[1] | pattern2[2] | pattern2[3] ;

  return candidates & ~(black | white);
}

Board get_candidates_simd128(Board black, Board white) {
  alignas(16) Board mask[4] = {
    white & HorizontalMask, white & HorizontalMask & VerticalMask,
    white & VerticalMask, white & HorizontalMask & VerticalMask
  };
  alignas(16) Board diff[4] = { 1, 7, 8, 9 };
  alignas(16) Board pattern[4] = { black, black, black, black };
  auto mm_mask1 = _mm_load_si128(reinterpret_cast<const __m128i*>(mask));
  auto mm_mask2 = _mm_load_si128(reinterpret_cast<const __m128i*>(mask+2));
  auto mm_diff1 = _mm_load_si128(reinterpret_cast<const __m128i*>(diff));
  auto mm_diff2 = _mm_load_si128(reinterpret_cast<const __m128i*>(diff+2));
  auto mm_pattern1 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern));
  auto mm_pattern2 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern+2));
  auto mm_pattern3 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern+4));
  auto mm_pattern4 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern+6));
  mm_pattern1 = _mm_and_si128(mm_mask1, _mm_sllv_epi64(mm_pattern1, mm_diff1));
  mm_pattern2 = _mm_and_si128(mm_mask2, _mm_sllv_epi64(mm_pattern2, mm_diff2));
  mm_pattern3 = _mm_and_si128(mm_mask1, _mm_srlv_epi64(mm_pattern3, mm_diff1));
  mm_pattern4 = _mm_and_si128(mm_mask2, _mm_srlv_epi64(mm_pattern4, mm_diff2));
  for(size_t i; i < 5; i++) {
    mm_pattern1 = _mm_or_si128(mm_pattern1, _mm_and_si128(mm_mask1, _mm_sllv_epi64(mm_pattern1, mm_diff1)));
    mm_pattern2 = _mm_or_si128(mm_pattern2, _mm_and_si128(mm_mask2, _mm_sllv_epi64(mm_pattern2, mm_diff2)));
    mm_pattern3 = _mm_or_si128(mm_pattern3, _mm_and_si128(mm_mask1, _mm_srlv_epi64(mm_pattern3, mm_diff1)));
    mm_pattern4 = _mm_or_si128(mm_pattern4, _mm_and_si128(mm_mask2, _mm_srlv_epi64(mm_pattern4, mm_diff2)));
  }
  mm_pattern1 = _mm_sllv_epi64(mm_pattern1, mm_diff1);
  mm_pattern2 = _mm_sllv_epi64(mm_pattern2, mm_diff2);
  mm_pattern3 = _mm_srlv_epi64(mm_pattern3, mm_diff1);
  mm_pattern4 = _mm_srlv_epi64(mm_pattern4, mm_diff2);
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern), _mm_or_si128(
    _mm_or_si128(mm_pattern1, mm_pattern2),
    _mm_or_si128(mm_pattern3, mm_pattern4)
  ));
  return (pattern[0] | pattern[1]) & ~ (black | white);
}

Board get_candidates_simd256(Board black, Board white) {
  alignas(32) Board mask[4] = {
    white & HorizontalMask, white & HorizontalMask & VerticalMask,
    white & VerticalMask, white & HorizontalMask & VerticalMask
  };
  alignas(32) Board diff[4] = { 1, 7, 8, 9 };
  alignas(32) Board pattern[4] = { black, black, black, black };
  auto mm_mask = _mm256_load_si256(reinterpret_cast<const __m256i*>(mask));
  auto mm_diff = _mm256_load_si256(reinterpret_cast<const __m256i*>(diff));
  auto mm_pattern1 = _mm256_load_si256(reinterpret_cast<const __m256i*>(pattern));
  auto mm_pattern3 = _mm256_load_si256(reinterpret_cast<const __m256i*>(pattern));
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
  //return get_candidates_nosimd(black, white);
  //return get_candidates_simd128(black, white);
  return get_candidates_simd256(black, white);
}
