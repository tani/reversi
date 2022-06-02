#include<cstdint>
#include<valarray>
#include<random>
#include<ctime>

#include<cassert>
#include<cstdio>

#include<string>
#include<sstream>
#include<tuple>

using Board = uint64_t;

// C++20 std::bit_floor
uint64_t bit_floor(uint64_t x) {
  return 1ull << (63 - __builtin_clzll(x));
  //return 1ull << static_cast<size_t>(std::log2(x));
}

// C++20 std::popcount
size_t popcount(uint64_t x) {
  return __builtin_popcountll(x);
  //return std::bitset<64>(x).count();
}

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

uint64_t HorizontalMask = (
    0b01111110ull << 8 * 7
  | 0b01111110ull << 8 * 6
  | 0b01111110ull << 8 * 5
  | 0b01111110ull << 8 * 4
  | 0b01111110ull << 8 * 3
  | 0b01111110ull << 8 * 2
  | 0b01111110ull << 8 * 1
  | 0b01111110ull << 8 * 0
);
uint64_t VerticalMask = (
    0b00000000ull << 8 * 7
  | 0b11111111ull << 8 * 6
  | 0b11111111ull << 8 * 5
  | 0b11111111ull << 8 * 4
  | 0b11111111ull << 8 * 3
  | 0b11111111ull << 8 * 2
  | 0b11111111ull << 8 * 1
  | 0b00000000ull << 8 * 0
);

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

double simulate(Board black, Board white, Board position) {
  std::mt19937 mt(std::time(NULL));
  size_t w = 0, t = 2 << 16; 
  Board reverse = get_reverse(black, white, position);
  Board black1 = black ^ reverse ^ position;
  Board white1 = white ^ reverse;
  for(size_t r = 0; r < t; r++) {
    Board black2 = black1, white2 = white1;
    size_t skip = 0;
    bool turn = false;
    while(skip < 2) {
      Board candidates = turn
        ? get_candidates(black2, white2)
        : get_candidates(white2, black2);
      skip += candidates == 0;
      size_t k = candidates && mt() % popcount(candidates);
      for(int j = 0; j < k; j++) {
        candidates &= ~(bit_floor(candidates));
      }
      Board position = bit_floor(candidates);
      Board reverse = turn
        ? get_reverse(black2, white2, position)
        : get_reverse(white2, black2, position);
      black2 ^= turn ? reverse ^ position : reverse;
      white2 ^= turn ? reverse : reverse ^ position;
      turn = !turn;
    }
    w += popcount(black2) > popcount(white2);
  }
  return 1.0 * w / t;
}

int main(int argc, char** argv) {
  auto [black, white] = parse(
    "        "
    "        "
    "        "
    "   ox   "
    "   xxx  "
    "        "
    "        "
    "        "
  );
  std::printf("Rate: %f", simulate(white, black, get_position(3, 5)));
  return 0;
}