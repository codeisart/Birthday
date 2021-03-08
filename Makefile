all: main

main: main.cpp
	g++-10 -std=c++17 -o main main.cpp
