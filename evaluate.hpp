#pragma once
#include "./board.hpp"
#include "./bit.hpp"
#include "./candidates.hpp"
#include "./reverse.hpp"
#include "./simulate.hpp"

double evaluate_monte_carlo_full(Board black, Board white, double probability = 1, size_t skip = 0, size_t depth = 0) {
  if (skip >= 2 || ~(black | white) == 0) {
    //puts(stringify(black, white, true).c_str());
    return depth % 2 == 0
      ? (popcount(black) > popcount(white) ? probability : 0)
      : (popcount(white) > popcount(black) ? probability : 0);
  }
  Board candidates = get_candidates(white, black);
  if(candidates == 0) {
    return evaluate_monte_carlo_full(white, black, probability, skip + 1, depth + 1);
  }
  double sum = 0;
  size_t n = popcount(candidates);
  for(int j = 0; j < n; j++) {
    Board position = bit_floor(candidates);
    Board reverse = get_reverse(white, black, position);
    Board black1 = black ^ reverse;
    Board white1 = white ^ reverse ^ position;
    sum += evaluate_monte_carlo_full(white1, black1, probability / n, 0, depth + 1);
    candidates &= ~position;
  }
  return sum;
}

double evaluate_monte_carlo_random(Board black, Board white, size_t n = 1024) {
  size_t w = 0; 
  for(size_t r = 0; r < n; r++) {
    auto [ black1, white1 ] = simulate(black, white);
    w += popcount(black1) > popcount(white1);
  }
  return (1.0 * w) / n;
}

constexpr double evaluate_win_loss(Board black, Board white) {
  return popcount(black) > popcount(white) ? 1.0
       : popcount(black) < popcount(white) ? 0.0
       : 0.5;
}
static_assert(evaluate_win_loss(0b001ull, 0b110) == 0, "");
static_assert(evaluate_win_loss(0b001ull, 0b100) == 0.5, "");
static_assert(evaluate_win_loss(0b111ull, 0b110) == 1, "");

double evaluate_static(Board black, Board white) {
  Board black_candidates = get_candidates(black, white);
  Board white_candidates = get_candidates(white, black);
  constexpr Board edge = board(
    0b11111111ull,
    0b10000001ull,
    0b10000001ull,
    0b10000001ull,
    0b10000001ull,
    0b10000001ull,
    0b10000001ull,
    0b11111111ull
  );
  constexpr Board corner = board(
    0b10000001ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b00000000ull,
    0b10000001ull
  );
  size_t diff = popcount(black_candidates) - popcount(white_candidates) + 64; // 0 -- 128
  size_t nedge = popcount(black_candidates & edge); // 0 -- 28
  size_t ncorner = popcount(black_candidates & corner); // 0 -- 4
  return (diff + nedge + ncorner) / 160.0;
}

using Eval = double(Board, Board);

// {black}番が終了した盤面が渡される。{white}番から最善手を{2n}回打ったときのEvalの結果を返す。
double evaluate_nega_alpha(Board black, Board white, Eval eval, size_t n = 5, size_t depth = 0, double a = 0, double b = 1) {
  if (n == 0) {
    return eval(black, white);
  }
  Board candidates = get_candidates(white, black);
  size_t m = popcount(candidates);
  for(int i = 0; i < m; i++) {
    Board position = bit_floor(candidates);
    Board reverse = get_reverse(white, black, position);
    Board black1 = black ^ reverse;
    Board white1 = white ^ reverse ^ position;
    a = std::max(a, -evaluate_nega_alpha(white1, black1, eval, n - (depth % 2), depth + 1, -b, -a));
    if (a >= b) {
      return a;
    }
  }
  return a;
}