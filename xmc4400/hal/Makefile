.PHONY: link

link: etherdrive.so

obj-m += etherdrive.o

LDFLAGS=$(shell xeno-config --ldflags --posix --no-auto-init)
LDFLAGS+=-L $(EMC2_HOME)/lib -luspace-xenomai

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -o $@ $(EXTRA_CFLAGS) -std=c++17 -c $<

include $(EMC2_HOME)/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	ln -f etherdrive.so $(EMC2_HOME)/rtlib/etherdrive.so
