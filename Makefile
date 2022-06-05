CXX := g++

all: reversi.out reversi.dbg reversi.s

reversi.out: main.cpp
	$(CXX) -o $@ -std=c++17 -flto -march=native -mtune=native -O3 $<

reversi.s: main.cpp
	$(CXX) -o $@ -std=c++17 -flto -march=native -mtune=native -O0 -S $<

reversi.dbg: main.cpp
	$(CXX) -g -o $@ -std=c++17 -march=native -mtune=native $< -lprofiler

reversi.wasm: main.cpp
	emcc -o $@ -mtune=native -march=native -std=c++17 -O3 $<

.PHONEY: clean

clean:
	rm -f reversi.{s,out,dbg}
