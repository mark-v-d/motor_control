.PHONY: link

link: etherdrive.so

obj-m += etherdrive.o

LDFLAGS=$(shell xeno-config --ldflags --posix --no-auto-init)

include /usr/src/linuxcnc/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	rm -f /usr/src/linuxcnc/rtlib/etherdrive.so
	ln etherdrive.so /usr/src/linuxcnc/rtlib/etherdrive.so
