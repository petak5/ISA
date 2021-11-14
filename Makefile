# ISA project
# Peter Urgoš (xurgos00)
# 11/2021

all: main

main:
	cd src && make && mv client ..

.PHONY: tar
tar:
	tar -cvf xurgos00.tar Makefile isa.pcap isa.lua manual.pdf src/Makefile src/*.cpp src/*.h
