#pragma once
#include"./bit.hpp"
#include"./board.hpp"
#include"./test.hpp"
#include <emmintrin.h>
#include <immintrin.h>

#if defined(__AVX2__)
#  if defined(__MSC_VER)
#    include<intrin.h>
#  else
#    include<x86intrin.h>
#  endif
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
  return get_reverse_simd256(black, white, position);
}
#elif defined(__EMSCRIPTEN__) && defined(__SSE2__)
#include<wasm_simd128.h>
Board get_reverse_simd128(Board black, Board white, Board position) {
  alignas(16) Board result[2], mask[4] = {
    HorizontalMask, HorizontalMask & VerticalMask,
    VerticalMask, VerticalMask & HorizontalMask
  };
  alignas(16) const size_t diff[4] = { 1, 7, 8, 9 };
  auto mm_result = wasm_i64x2_make(0, 0);
  for(int i = 0; i < 4; i++) {
    auto mm_pattern1 = wasm_i64x2_make(black, position);
    auto mm_pattern2 = wasm_i64x2_make(position, black);
    auto mm_mask = wasm_i64x2_make(white & mask[i], white & mask[i]);
    mm_pattern1 = wasm_v128_and(mm_mask, wasm_i64x2_shl(mm_pattern1, diff[i]));
    mm_pattern2 = wasm_v128_and(mm_mask, wasm_u64x2_shr(mm_pattern2, diff[i]));
    for(int j = 0; j < 5; j++) {
      mm_pattern1 = wasm_v128_or(mm_pattern1, wasm_v128_and(mm_mask, wasm_i64x2_shl(mm_pattern1, diff[i])));
      mm_pattern2 = wasm_v128_or(mm_pattern2, wasm_v128_and(mm_mask, wasm_u64x2_shr(mm_pattern2, diff[i])));
    }
    mm_result = wasm_v128_or(mm_result, wasm_v128_and(mm_pattern1, mm_pattern2));
  }
  wasm_v128_store(result, mm_result); 
  return result[0] | result[1];
}

Board get_reverse(Board black, Board white, Board position) {
  return get_reverse_simd128(black, white, position);
}
#elif defined(__SSE2__)
#  if defined(__MSC_VER)
#    include<intrin.h>
#  else
#    include<x86intrin.h>
#  endif
Board get_reverse_simd128(Board black, Board white, Board position) {
  alignas(16) Board result[2];
  alignas(16) const Board mask[4] = {
    HorizontalMask, HorizontalMask & VerticalMask,
    VerticalMask, VerticalMask & HorizontalMask
  };
  alignas(16) const size_t diff[4] = { 1, 7, 8, 9 };
  auto mm_result = _mm_setzero_si128();
  for(int i = 0; i < 4; i++) {
    auto mm_pattern1 = _mm_set_epi64x(black, position);
    auto mm_pattern2 = _mm_set_epi64x(position, black);
    auto mm_mask = _mm_set1_epi64x(white & mask[i]);
    mm_pattern1 = _mm_and_si128(mm_mask, _mm_slli_epi64(mm_pattern1, diff[i]));
    mm_pattern2 = _mm_and_si128(mm_mask, _mm_srli_epi64(mm_pattern2, diff[i]));
    for(int j = 0; j < 5; j++) {
      mm_pattern1 = _mm_or_si128(mm_pattern1, _mm_and_si128(mm_mask, _mm_slli_epi64(mm_pattern1, diff[i])));
      mm_pattern2 = _mm_or_si128(mm_pattern2, _mm_and_si128(mm_mask, _mm_srli_epi64(mm_pattern2, diff[i])));
    }
    mm_result = _mm_or_si128(mm_result, _mm_and_si128(mm_pattern1, mm_pattern2));
  }
  _mm_store_si128(reinterpret_cast<__m128i*>(result), mm_result); 
  return result[0] | result[1];
}

Board get_reverse(Board black, Board white, Board position) {
  return get_reverse_simd128(black, white, position);
}
#else
constexpr Board get_reverse_nosimd(Board black, Board white, Board position) {
  Board result = 0;
  const Board mask[4] = {
    white & HorizontalMask, white & HorizontalMask & VerticalMask,
    white & VerticalMask, white & VerticalMask & HorizontalMask
  };
  const size_t diff[4] = { 1, 7, 8, 9 };
  for(int i = 0; i < 4; i++) {
    Board pattern1 = mask[i] & (black << diff[i]);
    Board pattern2 = mask[i] & (position >> diff[i]); 
    Board pattern3 = mask[i] & (black >> diff[i]);
    Board pattern4 = mask[i] & (position << diff[i]); 
    for(int j = 0; j < 5; j++) {
      pattern1 |= mask[i] & (pattern1 << diff[i]);
      pattern2 |= mask[i] & (pattern2 >> diff[i]);
      pattern3 |= mask[i] & (pattern3 >> diff[i]);
      pattern4 |= mask[i] & (pattern4 << diff[i]);
    }
    result |= (pattern1 & pattern2) | (pattern3 & pattern4);
  }
  return result;
}

TEST_BEGIN(get_reverse)
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
constexpr Board position =
  board(
    0b00000000ull,
    0b00000000ull,
    0b00001000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull
  );
constexpr Board reverse =
  board(
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00001000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull
  );
static_assert(get_reverse_nosimd(black, white, position) == reverse, "");
TEST_END(get_reverse)

Board get_reverse(Board black, Board white, Board position) {
  return get_reverse_nosimd(black, white, position);
  //return get_reverse_simd256(black, white, position);
}
#endif
