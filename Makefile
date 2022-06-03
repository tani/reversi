all: reversi.out reversi_dbg.out

reversi.out: reversi.cpp
	g++ -o $@ -std=c++17 -march=native -O3 $<
reversi_dbg.out: reversi.cpp
	g++ -g -o $@ -std=c++17 -march=native $<

.PHONEY: clean

clean:
	rm reversi