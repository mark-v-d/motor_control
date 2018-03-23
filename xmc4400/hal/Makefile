.PHONY: link

link: etherdrive.so analogy.so

obj-m += etherdrive.o
obj-m += analogy.o
obj-m += homing.o

LDFLAGS=$(shell xeno-config --ldflags --posix --no-auto-init --mode-check)
LDFLAGS+=-L $(EMC2_HOME)/lib -luspace-xenomai -lanalogy

CXXFLAGS+=$(shell xeno-config --cflags --posix)

%.o: %.cpp
	$(ECHO) Compiling realtime $<
	$(Q)$(CC) -I ../include $(CXXFLAGS) -o $@ $(EXTRA_CFLAGS) -std=c++17 -c $<

include $(EMC2_HOME)/src/Makefile.modinc

clean:
	rm etherdrive.o etherdrive.so 

link:
	ln -f etherdrive.so $(EMC2_HOME)/rtlib/etherdrive.so
	ln -f analogy.so $(EMC2_HOME)/rtlib/analogy.so
