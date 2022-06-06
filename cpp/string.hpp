#pragma once
#include<cstdio>
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

void print(Board black, Board white) {
  const std::string fmt =
    "    a   b   c   d   e   f   g   h  \n"
    "  ┏━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┓\n"
    "1 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "2 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "3 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "4 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "5 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "6 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "7 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n"
    "8 ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃ %c ┃\n"
    "  ┗━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┛\n";
  char s[64];
  Board m = 1ull << 63;
  for(int i = 0; i < 64; i++) {
    s[i] = (black & m) ? 'x'
         : (white & m) ? 'o'
         : ' ';
    m >>= 1;
  }
  std::printf(fmt.c_str(),
    s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7],
    s[8], s[9], s[10], s[11], s[12], s[13], s[14], s[15],
    s[16], s[17], s[18], s[19], s[20], s[21], s[22], s[23],
    s[24], s[25], s[26], s[27], s[28], s[29], s[30], s[31],
    s[32], s[33], s[34], s[35], s[36], s[37], s[38], s[39],
    s[40], s[41], s[42], s[43], s[44], s[45], s[46], s[47],
    s[48], s[49], s[50], s[51], s[52], s[53], s[54], s[55],
    s[56], s[57], s[58], s[59], s[60], s[61], s[62], s[63]
  );
}