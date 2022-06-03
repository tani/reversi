#pragma once
#include<cstdint>

// C++20 std::bit_floor std::popcount
#if defined(__cplusplus) && __cplusplus >= 202002L
#include<bit>
  uint64_t bit_floor(uint64_t x) {
    return std::bit_floor(x);
  }
  size_t popcount(uint64_t x) {
    return std::popcount(x);
  }
#elif defined(__clang__) || defined(__GNUC__)
  uint64_t bit_floor(uint64_t x) {
    return 1ull << (64 - __builtin_clzll(x) - 1);
  }
  size_t popcount(uint64_t x) {
    return __builtin_popcountll(x);
  }
#else
#include<cmath>
#include<bitset>
  uint64_t bit_floor(uint64_t x) {
    return 1ull << static_cast<size_t>(std::log2(x));
  }
  size_t popcount(uint64_t x) {
    return std::bitset<64>(x).count();
  }
#endif
