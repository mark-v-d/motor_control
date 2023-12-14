.PHONY: link

link: etherdrive.so

obj-m += etherdrive.o

LDFLAGS+=-L $(EMC2_HOME)/lib

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -I ../include $(CXXFLAGS) -o $@ $(EXTRA_CFLAGS) -std=c++17 -c $<

include $(EMC2_HOME)/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	ln -f etherdrive.so $(EMC2_HOME)/rtlib/etherdrive.so

etherdrive.o: drive_io.h drive_packet.h packet.h raw_socket.h
