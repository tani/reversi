#include<cassert>
#include<cstdio>
#include<cmath>

#include"./bit.hpp"
#include"./board.hpp"
#include"./candidates.hpp"
#include"./reverse.hpp"
#include"./string.hpp"
#include"./random.hpp"
#include"./test.hpp"

constexpr Board get_position(size_t x, size_t y) {
  return 1ull << 63 >> (x + y * 8);
}
TEST_BEGIN(get_position)
const Board expected = 
  board(
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00100000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull
  );
static_assert(expected == get_position(2, 3));
TEST_END(get_position)

// {black}番が終了した盤面が渡される。{black}番の勝率を計算する。
double analyze(Board black, Board white, double probability = 1, size_t skip = 0, size_t depth = 0) {
  if (skip >= 2 || ~(black | white) == 0) {
    //puts(stringify(black, white, true).c_str());
    return depth % 2 == 0
      ? (popcount(black) > popcount(white) ? probability : 0)
      : (popcount(white) > popcount(black) ? probability : 0);
  }
  Board candidates = get_candidates(white, black);
  if(candidates == 0) {
    return analyze(white, black, probability, skip + 1, depth + 1);
  }
  double sum = 0;
  size_t n = popcount(candidates);
  for(int j = 0; j < n; j++) {
    Board position = bit_floor(candidates);
    Board reverse = get_reverse(white, black, position);
    Board black1 = black ^ reverse;
    Board white1 = white ^ reverse ^ position;
    sum += analyze(white1, black1, probability / n, 0, depth + 1);
    candidates &= ~position;
  }
  return sum;
}

// {black}番が終了した盤面が渡される。{white}番から最善手を{2n}回打ったときの勝率を返す。
double evaluate_nega_alpha(Board black, Board white, size_t n = 5, size_t depth = 0, double a = 0, double b = 1) {
  if (n == 0) {
    return analyze(black, white);
  }
  Board candidates = get_candidates(white, black);
  size_t m = popcount(candidates);
  for(int i = 0; i < m; i++) {
    Board position = bit_floor(candidates);
    Board reverse = get_reverse(white, black, position);
    Board black1 = black ^ reverse;
    Board white1 = white ^ reverse ^ position;
    a = std::max(a, -evaluate_nega_alpha(white1, black1, n - (depth % 2), depth + 1, -b, -a));
    if (a >= b) {
      return a;
    }
  }
  return a;
}

// {black}番が終了した盤面が渡される。ランダムに{2n}回進めたときの盤面を返す。
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

// {black}番が終了した盤面が渡される。ランダムに{n}回試行したときの勝率を返す。
double evaluate_monte_carlo(Board black, Board white, size_t n = 1024) {
  size_t w = 0; 
  for(size_t r = 0; r < n; r++) {
    auto [ black1, white1 ] = simulate(black, white);
    w += popcount(black1) > popcount(white1);
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
  auto [white1, black1] = simulate(white, black, 20);
  auto candidates = get_candidates(black1, white1);
  Board position = bit_floor(candidates);
  Board reverse = get_reverse(black, white, position);
  black1 ^= reverse ^ position;
  white1 ^= reverse;

  std::printf(" NegaAlphaRate: %lg\n", evaluate_nega_alpha(black1, white1));
  std::printf("MonteCarloRate: %lg\n", evaluate_monte_carlo(black1, white1));
  //std::printf("Rate: %f", evaluate(black, white, get_position(3, 2)));
  return 0;
}
