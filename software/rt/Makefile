LDFLAGS=-lpthread
CFLAGS=-g
CPPFLAGS=-I.
CXXFLAGS= -g --std=c++20 -lfmt

all: voltage_setpoints current_setpoints motion_setpoints set_config speed_setpoints

#INCLUDES=drive_io.h drive_packet.h packet.h raw_socket.h

current_setpoints: $(INCLUDES)

motion_setpoints:  $(INCLUDES) ss.h
