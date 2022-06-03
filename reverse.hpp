#pragma once
#include<valarray>
#include<cstdint>
#include"./board.hpp"

#if defined(__MSC_VER)
#  include<intrin.h>
#else
#  include<x86intrin.h>
#endif

Board get_reverse_nosimd(Board black, Board white, Board position) {
  std::valarray<Board> mask = {
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
  };

  std::valarray<Board> diff = {
    1, 7, 8, 9,
    1, 7, 8, 9
  };

  std::valarray<Board> pattern1 = {
    position, position, position, position,
    black, black, black, black
  };

  std::valarray<Board> pattern2 = {
    black, black, black, black,
    position, position, position, position
  };

  pattern1 = mask & (pattern1 << diff);
  pattern2 = mask & (pattern2 >> diff);
  for(size_t i = 0; i < 5; i++) {
    pattern1 |= mask & (pattern1 << diff);
    pattern2 |= mask & (pattern2 >> diff);
  }

  std::valarray<Board> pattern3 = pattern1 & pattern2;

  return pattern3[0] | pattern3[1] | pattern3[2] | pattern3[3] |
         pattern3[4] | pattern3[5] | pattern3[6] | pattern3[7];
}

Board get_reverse_simd128(Board black, Board white, Board position) {
  alignas(16) Board mask[8] = {
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
  };
  alignas(16) Board diff[8] = {
    1, 7, 8, 9,
    1, 7, 8, 9,
  };
  alignas(16) Board pattern[2];
  alignas(16) Board pattern1[16] = {
    position, position, position, position,
    black, black, black, black,
  };
  alignas(16) Board pattern2[16] = {
    black, black, black, black,
    position, position, position, position
  };
  __m128i mm_mask[4];
  __m128i mm_diff[4];
  __m128i mm_pattern;
  __m128i mm_pattern1[4];
  __m128i mm_pattern2[4];
  __m128i mm_pattern3[4];
  for(size_t i = 0; i < 4; i++) {
    mm_mask[i] = _mm_load_si128(reinterpret_cast<__m128i*>(mask + i * 2));
    mm_diff[i] = _mm_load_si128(reinterpret_cast<__m128i*>(diff + i * 2));
    mm_pattern1[i] = _mm_load_si128(reinterpret_cast<__m128i*>(pattern1 + i * 2));
    mm_pattern2[i] = _mm_load_si128(reinterpret_cast<__m128i*>(pattern2 + i * 2));
  }
  for(size_t i = 0; i < 4; i++) {
    mm_pattern1[i] = _mm_and_si128(mm_mask[i], _mm_sllv_epi64(mm_pattern1[i], mm_diff[i]));
    mm_pattern2[i] = _mm_and_si128(mm_mask[i], _mm_srlv_epi64(mm_pattern2[i], mm_diff[i]));
  }
  for(size_t i = 0; i < 5; i++) {
    for(size_t j = 0; j < 4; j++) {
      mm_pattern1[j] = _mm_or_si128(mm_pattern1[j], _mm_and_si128(mm_mask[j], _mm_sllv_epi64(mm_pattern1[j], mm_diff[j])));
      mm_pattern2[j] = _mm_or_si128(mm_pattern2[j], _mm_and_si128(mm_mask[j], _mm_srlv_epi64(mm_pattern2[j], mm_diff[j])));
    }
  }
  for(size_t i = 0; i < 4; i++) {
    mm_pattern3[i] = _mm_and_si128(mm_pattern1[i], mm_pattern2[i]);
  }
  mm_pattern = _mm_or_si128(
    _mm_or_si128(mm_pattern3[0], mm_pattern3[1]),
    _mm_or_si128(mm_pattern3[2], mm_pattern3[3])
  );
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern), mm_pattern);
  return pattern[0] | pattern[1];
}

Board get_reverse_simd256(Board black, Board white, Board position) {
  alignas(32) Board mask[8] = {
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
  };
  alignas(32) Board diff[8] = {
    1, 7, 8, 9,
    1, 7, 8, 9,
  };
  alignas(32) Board pattern[4];
  alignas(32) Board pattern1[16] = {
    position, position, position, position,
    black, black, black, black,
  };
  alignas(32) Board pattern2[16] = {
    black, black, black, black,
    position, position, position, position
  };
  __m256i mm_mask[2];
  __m256i mm_diff[2];
  __m256i mm_pattern;
  __m256i mm_pattern1[2];
  __m256i mm_pattern2[2];
  __m256i mm_pattern3[2];
  for(size_t i = 0; i < 2; i++) {
    mm_mask[i] = _mm256_load_si256(reinterpret_cast<__m256i*>(mask + i * 4));
    mm_diff[i] = _mm256_load_si256(reinterpret_cast<__m256i*>(diff + i * 4));
    mm_pattern1[i] = _mm256_load_si256(reinterpret_cast<__m256i*>(pattern1 + i * 4));
    mm_pattern2[i] = _mm256_load_si256(reinterpret_cast<__m256i*>(pattern2 + i * 4));
  }
  for(size_t i = 0; i < 2; i++) {
    mm_pattern1[i] = _mm256_and_si256(mm_mask[i], _mm256_sllv_epi64(mm_pattern1[i], mm_diff[i]));
    mm_pattern2[i] = _mm256_and_si256(mm_mask[i], _mm256_srlv_epi64(mm_pattern2[i], mm_diff[i]));
  }
  for(size_t i = 0; i < 5; i++) {
    for(size_t j = 0; j < 2; j++) {
      mm_pattern1[j] = _mm256_or_si256(mm_pattern1[j], _mm256_and_si256(mm_mask[j], _mm256_sllv_epi64(mm_pattern1[j], mm_diff[j])));
      mm_pattern2[j] = _mm256_or_si256(mm_pattern2[j], _mm256_and_si256(mm_mask[j], _mm256_srlv_epi64(mm_pattern2[j], mm_diff[j])));
    }
  }
  for(size_t i = 0; i < 2; i++) {
    mm_pattern3[i] = _mm256_and_si256(mm_pattern1[i], mm_pattern2[i]);
  }
  mm_pattern = _mm256_or_si256(mm_pattern3[0], mm_pattern3[1]);
  _mm256_store_si256(reinterpret_cast<__m256i*>(pattern), mm_pattern);
  return pattern[0] | pattern[1] | pattern[2] | pattern[3];
}

Board get_reverse(Board black, Board white, Board position) {
  //return get_reverse_nosimd(black, white, position);
  //return get_reverse_simd128(black, white, position);
  return get_reverse_simd256(black, white, position);
}
