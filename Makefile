SRC=main.cpp

all: build run
build:
	g++ $(SRC) -std=c++17
run:
	./a.out

window:
	g++ window.cpp -std=c++17 -lsfml-window -lsfml-system -lsfml-graphics
