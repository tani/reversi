CXX := g++

all: reversi.out reversi.dbg reversi.s

reversi.out: main.cpp
	$(CXX) -o $@ -std=c++17 -flto -msse2 -mtune=native -O3 $<

reversi.s: main.cpp
	$(CXX) -o $@ -std=c++17 -flto -march=native -mtune=native -O0 -S $<

reversi.dbg: main.cpp
	$(CXX) -pg -o $@ -std=c++17 -march=native -mtune=native $<

reversi.wasm: main.cpp
	emcc -o $@ -std=c++17 -msse2 -msimd128 -flto -mtune=native -march=native -O3 $<

.PHONEY: clean

clean:
	rm -f *.s *.dbg *.out *.wasm
