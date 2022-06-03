#include <array>
#include<cstdint>
#include<valarray>
#include<ctime>
#include<unordered_map>
#include<utility>
#include<bitset>

#include<cassert>
#include<cstdio>

#include<string>
#include<sstream>
#include<tuple>

using Board = uint64_t;

// C++20 std::bit_floor std::popcount
#if defined(__cplusplus) && __cplusplus >= 202002L
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
#include<bitset>
#include<cmath>
  uint64_t bit_floor(uint64_t x) {
    return 1ull << static_cast<size_t>(std::log2(x));
  }
  size_t popcount(uint64_t x) {
    return std::bitset<64>(x).count();
  }
#endif

Board get_position(size_t x, size_t y) {
  return 1ull << (x + y * 8);
}

std::tuple<Board, Board> parse(const std::string& board) {
  Board black = 0, white = 0, mask = 1ull << 63;
  for(size_t i = 0; i < 64; i++) {
    switch (board.at(i)) {
      case 'x': black |= mask; break;
      case 'o': white |= mask; break;
      case '\n': i++;
    }
    mask >>= 1;
  }
  return { black, white };
}

std::string stringify(Board black, Board white, bool newline = false) {
  std::stringstream ss;
  Board mask = 1ull << 63;
  for(size_t i = 0; i < 64; i++) {
    if ((black & mask)) {
      ss << 'x';
    } else if ((white & mask)) {
      ss << 'o';
    } else {
      ss << ' ';
    }
    if (newline && i % 8 == 7) {
      ss << '\n';
    }
    mask >>= 1;
  }
  return ss.str();
}

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

#include<immintrin.h>
Board get_candidates_simd(Board black, Board white) {
  alignas(16) const std::array<Board, 4> mask = {
    white & HorizontalMask,
    white & HorizontalMask & VerticalMask,
    white & VerticalMask,
    white & HorizontalMask & VerticalMask,
  };
  alignas(16) const std::array<Board, 4> diff = { 1, 7, 8, 9 };
  alignas(16) std::array<Board, 8> pattern = {
    black, black, black, black, black, black, black, black
  };
  auto mask1 = _mm_load_si128(reinterpret_cast<const __m128i*>(mask.data()));
  auto mask2 = _mm_load_si128(reinterpret_cast<const __m128i*>(mask.data()+2));
  auto diff1 = _mm_load_si128(reinterpret_cast<const __m128i*>(diff.data()));
  auto diff2 = _mm_load_si128(reinterpret_cast<const __m128i*>(diff.data()+2));
  auto pattern1 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern.data()));
  auto pattern2 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern.data()+2));
  auto pattern3 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern.data()+4));
  auto pattern4 = _mm_load_si128(reinterpret_cast<const __m128i*>(pattern.data()+6));
  pattern1 = _mm_and_si128(mask1, _mm_sllv_epi64(pattern1, diff1));
  pattern2 = _mm_and_si128(mask2, _mm_sllv_epi64(pattern2, diff2));
  pattern3 = _mm_and_si128(mask1, _mm_srlv_epi64(pattern3, diff1));
  pattern4 = _mm_and_si128(mask2, _mm_srlv_epi64(pattern4, diff2));
  for(size_t i; i < 5; i++) {
    pattern1 = _mm_or_si128(pattern1, _mm_and_si128(mask1, _mm_sllv_epi64(pattern1, diff1)));
    pattern2 = _mm_or_si128(pattern2, _mm_and_si128(mask2, _mm_sllv_epi64(pattern2, diff2)));
    pattern3 = _mm_or_si128(pattern3, _mm_and_si128(mask1, _mm_srlv_epi64(pattern3, diff1)));
    pattern4 = _mm_or_si128(pattern4, _mm_and_si128(mask2, _mm_srlv_epi64(pattern4, diff2)));
  }
  pattern1 = _mm_sllv_epi64(pattern1, diff1);
  pattern2 = _mm_sllv_epi64(pattern2, diff2);
  pattern3 = _mm_srlv_epi64(pattern3, diff1);
  pattern4 = _mm_srlv_epi64(pattern4, diff2);
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern.data()), pattern1);
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern.data()+2), pattern2);
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern.data()+4), pattern3);
  _mm_store_si128(reinterpret_cast<__m128i*>(pattern.data()+6), pattern4);
  return pattern[0] | pattern[1] | pattern[2] | pattern[3] |
         pattern[4] | pattern[5] | pattern[6] | pattern[7];
}
Board get_candidates(Board black, Board white) {
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

Board get_reverse(Board black, Board white, Board position) {
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

uint32_t xor32(void) {
  static uint32_t y = std::time(NULL);
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}


double analyze(Board black, Board white, double probability = 1, bool turn = true, size_t skip = 0) {
  if (skip >= 2 || ~(black | white) == 0) {
    return popcount(black) > popcount(white) ? probability : 0;
  }
  Board candidates = get_candidates_simd(turn ? black : white, turn ? white : black);
  if(!candidates) {
    return analyze(black, white, probability, !turn, skip + 1);
  }
  double sum = 0;
  size_t n = popcount(candidates);
  for(int j = 0; j < n; j++) {
    Board position = bit_floor(candidates);
    Board reverse = get_reverse(turn ? black : white, turn ? white : black, position);
    Board black1 = turn ? black ^ reverse ^ position : black ^ reverse;
    Board white1 = turn ? white ^ reverse : white ^ reverse ^ position;
    sum += analyze(black1, white1, probability / n, !turn, 0);
    candidates &= ~position;
  }
  return sum;
}

std::tuple<Board, Board, bool> simulate(Board black, Board white, size_t n = 64) {
  size_t skip = 0;
  bool turn = false;
  while(skip < 2 && popcount(black | white) < n) {
    Board candidates = turn
      ? get_candidates(black, white)
      : get_candidates(white, black);
    skip = candidates ? 0 : (skip + 1);
    size_t k = candidates && xor32() % popcount(candidates);
    for(int j = 0; j < k; j++) {
      candidates &= ~(bit_floor(candidates));
    }
    Board position = bit_floor(candidates);
    Board reverse = turn
      ? get_reverse(black, white, position)
      : get_reverse(white, black, position);
    black ^= turn ? reverse ^ position : reverse;
    white ^= turn ? reverse : reverse ^ position;
    turn = !turn;
  }
  return {black, white, turn};
} 

double evaluate(Board black, Board white, Board position, size_t n = 1024) {
  size_t w = 0; 
  Board reverse = get_reverse(black, white, position);
  Board black1 = black ^ reverse ^ position;
  Board white1 = white ^ reverse;
  for(size_t r = 0; r < n; r++) {
    auto [ black2, white2, _ ] = simulate(black1, white1);
    w += popcount(black2) > popcount(white2);
  }
  return (1.0 * w) / n;
}

int main(int argc, char** argv) {
  auto [black, white] = parse(
    "        "
    "        "
    "        "
    "   ox   "
    "   xo   "
    "        "
    "        "
    "        "
  );
  auto [black1, white1, turn] = simulate(black, white, 60);
  std::printf("Rate: %lg\n", analyze(black1, white1, turn));
  //std::printf("Rate: %f", evaluate(black, white, get_position(3, 2)));
  return 0;
}