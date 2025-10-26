.PHONY: link

link: etherdrive.so statespace_3.so speed_controller.so partial_absolute.so

obj-m += etherdrive.o statespace_3.o speed_controller.o partial_absolute.o

LDFLAGS+=-L $(EMC2_HOME)/lib
CXXFLAGS+=-Wno-ignored-attributes

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -I ../include $(CXXFLAGS) -o $@ $(EXTRA_CFLAGS) -std=c++17 -c $<

include $(EMC2_HOME)/src/Makefile.modinc

clean:
	rm *.o *.so 

link:
	cp etherdrive.so $(EMC2_HOME)/rtlib/etherdrive.so
	cp statespace_3.so $(EMC2_HOME)/rtlib/statespace_3.so
	cp speed_controller.so $(EMC2_HOME)/rtlib/speed_controller.so
	cp partial_absolute.so $(EMC2_HOME)/rtlib/partial_absolute.so

etherdrive.o: drive_io.h drive_packet.h packet.h raw_socket.h

statespace_3.o: ss.h
