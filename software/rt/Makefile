LDFLAGS=-lpthread
CFLAGS=-g
CPPFLAGS=-I.
CXXFLAGS= -g --std=c++20 -lfmt

all: current_setpoints motion_setpoints set_config

#INCLUDES=drive_io.h drive_packet.h packet.h raw_socket.h

current_setpoints: $(INCLUDES)

motion_setpoints:  $(INCLUDES) ss.h
