all: reversi.out reversi_dbg.out

reversi.out: main.cpp
	g++ -o $@ -std=c++17 -flto -march=native -mtune=native -O3 $<

reversi.s: main.cpp
	g++ -o $@ -std=c++17 -flto -march=native -mtune=native -O3 -S $<

reversi_dbg.out: main.cpp
	g++ -g -o $@ -std=c++17 -march=native $<

.PHONEY: clean

clean:
	rm -f reversi_dbg.out reversi.out reversi.s
