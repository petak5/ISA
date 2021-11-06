# ISA project - client implementing a protocol
# Peter Urgoš (xurgos00)
# 11/2021

CC=g++
CFLAGS=-g -std=c++20 -pedantic -Wall -Wextra -O2

MODULES=main.o params.o response.o tools.o base64.o

all: main

main: $(MODULES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: main
	./main

.PHONY: clean
clean:
	rm -rf xurgos00.zip *.o main
