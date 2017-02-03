.PHONY: link

link: etherdrive.so

obj-m += etherdrive.o

LDFLAGS=$(shell xeno-config --ldflags --posix --no-auto-init)

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -o $@ $(EXTRA_CFLAGS) -c $<

include /usr/src/linuxcnc/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	ln -f etherdrive.so /usr/src/linuxcnc/rtlib/etherdrive.so
