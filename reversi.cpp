#include<valarray>
#include<string>
#include<sstream>
#include<iostream>
#include<random>
#include<tuple>
#include<bit>

uint64_t get_position(size_t x, size_t y) {
  return 1ull << (x + y * 8);
}

std::tuple<uint64_t, uint64_t> parse(const std::string& board) {
  uint64_t black = 0, white = 0, mask = 1ull << 63;
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

std::string stringify(uint64_t black, uint64_t white, bool newline = false) {
  std::stringstream ss;
  uint64_t mask = 1ull << 63;
  for(size_t i = 0; i < 64; i++) {
    if (black & mask) {
      ss << "x";
    } else if (white & mask) {
      ss << "o";
    } else {
      ss << " ";
    }
    if (newline && i % 8 == 7) {
      ss << std::endl;
    }
    mask >>= 1;
  }
  return ss.str();
}

constexpr uint64_t HorizonTalMask = (
        0b01111110ull << 8 * 7
      | 0b01111110ull << 8 * 6
      | 0b01111110ull << 8 * 5
      | 0b01111110ull << 8 * 4
      | 0b01111110ull << 8 * 3
      | 0b01111110ull << 8 * 2
      | 0b01111110ull << 8 * 1
      | 0b01111110ull << 8 * 0
);

constexpr uint64_t VerticalMask = (
        0b00000000ull << 8 * 7
      | 0b11111111ull << 8 * 6
      | 0b11111111ull << 8 * 5
      | 0b11111111ull << 8 * 4
      | 0b11111111ull << 8 * 3
      | 0b11111111ull << 8 * 2
      | 0b11111111ull << 8 * 1
      | 0b00000000ull << 8 * 0
);

inline uint64_t get_candidates(uint64_t black, uint64_t white) {
  std::valarray<uint64_t> mask = {
    white & HorizonTalMask,
    white & HorizonTalMask & VerticalMask,
    white & VerticalMask,
    white & HorizonTalMask & VerticalMask,
  };

  uint64_t result = 0;
  std::valarray<uint64_t>
    pattern1 = {black,black,black,black},
    pattern2 = {black,black,black,black},
    diff = { 1, 7, 8, 9 };

  for(size_t i = 0; i < 6; i++) {
    pattern1 |= mask & (pattern1 << diff);
    pattern2 |= mask & (pattern2 >> diff);
  }

  pattern1 <<= diff; 
  pattern2 >>= diff; 

  result |= pattern1[0] | pattern1[1] | pattern1[2] | pattern1[3] |
            pattern2[0] | pattern2[1] | pattern2[2] | pattern2[3];

  return result & ~(black | white);
}

inline uint64_t get_reverse(uint64_t black, uint64_t white, uint64_t position) {
  std::valarray<uint64_t> mask = {
    white & HorizonTalMask,
    white & HorizonTalMask & VerticalMask,
    white & VerticalMask,
    white & HorizonTalMask & VerticalMask,
    white & HorizonTalMask,
    white & HorizonTalMask & VerticalMask,
    white & VerticalMask,
    white & HorizonTalMask & VerticalMask,
  };

  uint64_t result = 0;
  std::valarray<uint64_t> diff = { 1, 7, 8, 9, 1, 7, 8, 9 };

  std::valarray<uint64_t> pattern1 = {
    position,position,position,position,
    black,black,black,black,
  };

  std::valarray<uint64_t> pattern2 = {
    black,black,black,black,
    position,position,position,position,
  };

  for(size_t i = 0; i < 6; i++) {
    pattern1 |= mask & (pattern1 << diff);
    pattern2 |= mask & (pattern2 >> diff);
  }

  std::valarray<uint64_t> pattern3 = pattern1 & pattern2;

  return pattern3[0] | pattern3[1] | pattern3[2] | pattern3[3] |
         pattern3[4] | pattern3[5] | pattern3[6] | pattern3[7];
}

static uint32_t y = []{
  std::random_device random;
  return random();
}();

inline uint32_t xor32(void) {
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

double simulate_montecarlo(uint64_t black, uint64_t white, uint64_t position) {
  size_t w = 0, t = 1024; 
  uint64_t reverse = get_reverse(black, white, position);
  black ^= reverse ^ position;
  white ^= reverse;
  size_t m = 64 - std::popcount(black | white);
  for(size_t r = 0; r < t; r++) {
    uint64_t black1 = black, white1 = white;
    for(size_t i = 0; i < m; i++) {
      uint64_t candidates = i % 2 == 0
        ? get_candidates(white1, black1)
        : get_candidates(black1, white1);
      size_t k = xor32() % std::popcount(candidates);
      for(int j = 0; j < k; j++) {
        candidates -= std::bit_floor(candidates);
      }
      uint64_t position = std::bit_floor(candidates);
      uint64_t reverse = i % 2 == 0
        ? get_reverse(white1, black1, position)
        : get_reverse(black1, white1, position);
      black1 ^= reverse ^ position;
      white1 ^= reverse;
    }
    if (std::popcount(black1) > std::popcount(white1)) {
      w++;
    }
  }
  return 1.0 * w / t;
}

double simulate_minimax(uint64_t black, uint64_t white, uint64_t position, uint64_t depth, uint64_t alpha, uint64_t beta) {
}
