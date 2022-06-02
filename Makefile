all: reversi

reversi: reversi.cpp
	g++ -o $@ -std=c++17 -march=native -O3 $<

.PHONEY: clean

clean:
	rm reversi