#pragma once
#include<string>
#include<sstream>
#include<tuple>
#include"./board.hpp"

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

