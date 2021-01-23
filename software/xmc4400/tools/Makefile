CXXFLAGS=-std=c++14 -g $(shell xeno-config --posix --cflags --ldflags)

convert_header: convert_header.cpp

asc: asc.cpp
	g++ -std=c++11 -o asc asc.cpp  -lboost_system
