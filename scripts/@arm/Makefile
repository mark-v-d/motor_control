CXXFLAGS=-std=c++17 -g
LDLIBS+=-static $(shell PKG_CONFIG_PATH=/usr/src/libelfin/elf:/usr/src/libelfin/dwarf;pkg-config --cflags --libs libdwarf++) -ldwarf -lelf  

.PHONY: all

all: decode parse

test: test.cpp statemachine.cpp
