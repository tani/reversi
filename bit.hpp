#pragma once
#include<cstdint>
#include<cstddef>
#include"./test.hpp"

const uint64_t m55555555 = 0x5555'5555'5555'5555ull;
const uint64_t m33333333 = 0x3333'3333'3333'3333ull;
const uint64_t m0F0F0F0F = 0x0F0F'0F0F'0F0F'0F0Full;
const uint64_t m00FF00FF = 0x00FF'00FF'00FF'00FFull;
const uint64_t m0000FFFF = 0x0000'FFFF'0000'FFFFull;
const uint64_t mFFFFFFFF = 0x0000'0000'FFFF'FFFFull;

constexpr uint64_t bit_reverse(uint64_t x) {
  x = (x & ~m55555555) >>  1 | (x & m55555555) <<  1;
  x = (x & ~m33333333) >>  2 | (x & m33333333) <<  2;
  x = (x & ~m0F0F0F0F) >>  4 | (x & m0F0F0F0F) <<  4;
  x = (x & ~m00FF00FF) >>  8 | (x & m00FF00FF) <<  8;
  x = (x & ~m0000FFFF) >> 16 | (x & m0000FFFF) << 16;
  x = (x & ~mFFFFFFFF) >> 32 | (x & mFFFFFFFF) << 32;
  return x;
}

constexpr uint64_t bit_shift_left(uint64_t a, size_t b) {
  return a << b;
};

constexpr uint64_t bit_shift_right(uint64_t a, size_t b) {
  return a >> b;
};

#if defined(__cplusplus) && __cplusplus >= 202002L
#include<bit>
  constexpr uint64_t bit_floor(uint64_t x) {
    return std::bit_floor(x);
  }
  constexpr size_t popcount(uint64_t x) {
    return std::popcount(x);
  }
#elif defined(__clang__) || defined(__GNUC__)
  constexpr uint64_t bit_floor(uint64_t x) {
    return 1ull << (64 - __builtin_clzll(x) - 1);
  }
  constexpr size_t popcount(uint64_t x) {
    return __builtin_popcountll(x);
  }
#else
  constexpr size_t popcount(uint64_t x) {
    x = (x & m55555555) + ((x >>  1) & m55555555);
    x = (x & m33333333) + ((x >>  2) & m33333333);
    x = (x & m0F0F0F0F) + ((x >>  4) & m0F0F0F0F);
    x = (x & m00FF00FF) + ((x >>  8) & m00FF00FF);
    x = (x & m0000FFFF) + ((x >> 16) & m0000FFFF);
    x = (x & mFFFFFFFF) + ((x >> 32) & mFFFFFFFF);
    return x;
  }
  
  constexpr uint64_t bit_floor(uint64_t x) {
    if (x & ~m55555555) x &= ~m55555555;
    if (x & ~m33333333) x &= ~m33333333;
    if (x & ~m0F0F0F0F) x &= ~m0F0F0F0F;
    if (x & ~m00FF00FF) x &= ~m00FF00FF;
    if (x & ~m0000FFFF) x &= ~m0000FFFF;
    if (x & ~mFFFFFFFF) x &= ~mFFFFFFFF;
    return x;
  }
#endif

TEST_BEGIN(bit)
static_assert(popcount(0b001001010100ull) == 4);
static_assert(bit_floor(0b001001010100ull) == 0b001000000000ull);
static_assert(bit_reverse(0xf000'0000'0000'0000ull) == 0x0000'0000'0000'000full, "");
TEST_END(bit)
