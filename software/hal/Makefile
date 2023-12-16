.PHONY: link

link: etherdrive.so statespace_3.so

obj-m += etherdrive.o statespace_3.o

LDFLAGS+=-L $(EMC2_HOME)/lib
CXXFLAGS+=-Wno-ignored-attributes

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -I ../include $(CXXFLAGS) -o $@ $(EXTRA_CFLAGS) -std=c++17 -c $<

include $(EMC2_HOME)/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	ln -f etherdrive.so $(EMC2_HOME)/rtlib/etherdrive.so
	ln -f statespace_3.so $(EMC2_HOME)/rtlib/statespace_3.so

statespace_3.o etherdrive.o: drive_io.h drive_packet.h packet.h raw_socket.h

statespace_3: ss.h
