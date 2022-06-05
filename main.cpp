#include<cassert>
#include<cstdio>
#include<cmath>
#include <cstdlib>
#include<iostream>

#include"./board.hpp"
#include"./string.hpp"
#include"./test.hpp"
#include"./simulate.hpp"
#include"./evaluate.hpp"
#include "bit.hpp"
#include "candidates.hpp"
#include "reverse.hpp"

constexpr Board get_position(char x, char y) {
  return 1ull << 63 >> ((x - 'a') + (y - '1') * 8);
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
static_assert(expected == get_position('c', '4'));
TEST_END(get_position)

Board search(Board black, Board white, Eval evaluate) {
  Board candidates = get_candidates(white, black);
  Board position = 0ull;
  Board best_position = 0ull;
  double best_score = 0;
  while((position = bit_floor(candidates))) {
    Board reverse = get_reverse(white, black, position);
    Board black1 = black ^ reverse;
    Board white1 = white ^ reverse ^ position;
    double score = evaluate(white1, black1);
    best_position = score > best_score ? position : best_position;
    best_score = std::max(best_score, score);
    candidates &= ~position;
  }
  return best_position;
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
  print(black, white);
  Board position, reverse;
  char x, y;
  while(true) {
    printf("(%2ld/%2ld) YOU> ", popcount(black), popcount(white));
    std::cin >> x >> y;
    if (x == 'q' || y == 'q') {
      exit(0);
    }
    position = get_position(x, y);
    reverse = get_reverse(black, white, position);
    black ^= reverse ^ position;
    white ^= reverse;
    print(black, white);
    position = search(black, white, [](auto black, auto white){ return evaluate_monte_carlo_random(black, white);});
    if (position) {
      x = (int)std::log2(position) % 8;
      y = (int)std::log2(position) / 8;
      printf("(%2ld/%2ld) COM> %c%c\n", popcount(black), popcount(white), (int)std::log2(position) % 8 + 'a', y + '1');
      reverse = get_reverse(white, black, position);
      black ^= reverse;
      white ^= reverse ^ position;
      print(black, white);
    } else {
      printf("(%2ld/%2ld) COM> PASS\n", popcount(black), popcount(white));
      print(black, white);
    }
  }
  //auto [white1, black1] = simulate(white, black, 20);
  //auto candidates = get_candidates(black1, white1);
  //Board position = bit_floor(candidates);
  //Board reverse = get_reverse(black, white, position);
  //black1 ^= reverse ^ position;
  //white1 ^= reverse;

  //Board best_position = search(black, white, [](auto black, auto white){
  //  return evaluate_nega_alpha(black, white, [](auto black, auto white){
  //    return evaluate_monte_carlo_random(black, white, 1000);
  //  }, 2);
  //});
  //print(best_position, 0ull);
  //std::printf(" NegaAlphaRate: %lg\n", evaluate_nega_alpha(black1, white1, [](auto black, auto white){return evaluate_monte_carlo_full(black, white);}));
  //std::printf(" NegaAlphaRate: %lg\n", evaluate_nega_alpha(black1, white1, [](auto black, auto white){return evaluate_static(black, white);}));
  //std::printf(" NegaAlphaRate: %lg\n", evaluate_nega_alpha(black1, white1, [](auto black, auto white){return evaluate_win_loss(black, white);}, 10));
  //std::printf("MonteCarloRate: %lg\n", evaluate_monte_carlo_random(black1, white1));
  return 0;
}