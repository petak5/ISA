# ISA project - client implementing a protocol
# Peter Urgoš (xurgos00)
# 11/2021

CC=g++
CFLAGS=-g -std=c++17 -pedantic -Wall -Wextra -O2

all: main

main: main.o
	$(CC) $(CFLAGS) main.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

run: main
	./main

.PHONY: clean
clean:
	rm -rf xurgos00.zip *.o main
