SRC=main.cpp

all: build run
build:
	g++ $(SRC) -std=c++17
run:
	./a.out
