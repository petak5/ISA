# ISA project - client implementing a protocol
# Peter Urgoš (xurgos00)
# 11/2021

CC=g++
CFLAGS=-g -std=c++17 -pedantic -Wall -Wextra -O2

all: main

main: main.o params.o
	$(CC) $(CFLAGS) main.o params.o -o main

main.o: main.cpp main.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

params.o: params.cpp params.h
	$(CC) $(CFLAGS) -c params.cpp -o params.o

run: main
	./main

.PHONY: clean
clean:
	rm -rf xurgos00.zip *.o main
