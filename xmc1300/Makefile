DEVICE=XMC1301
OBJS=main.o startup_XMC1300.o system_XMC1300.o

LDSCRIPT=xmc1300.ld

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src

CPPFLAGS+=-g -I ./include
CPPFLAGS+=-I src
CPPFLAGS+=-I ./Libraries/CMSIS/Infineon/XMC1300_series/Include/
CPPFLAGS+=-I ./Libraries/CMSIS/Include/
CPPFLAGS+=-DCLOCK_CRYSTAL_FREQUENCY=12000000
CPPFLAGS+=-D $(DEVICE)_$(PACKAGE)x$(MEMORY)

CPU = -g -mthumb -mcpu=cortex-m0

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU) -std=c++17 -O
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU) -nostartfiles -specs=nosys.specs

%.o: Makefile

.%.d: %.c Makefile
	$(CC) $(CPPFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.s
	touch $@ 

.%.d: %.S
	touch $@ 

%.elf: $(LDSCRIPT) 
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld -o $@

%.pre: %.cpp
	$(CPP) $(CPPFLAGS) $< > $@

main.elf: $(OBJS) 
	$(CXX) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld $+ -o $@

xmc1300.bin: main.elf
	$(CROSS)objcopy --adjust-vma=-0x20000200 -O binary -S $< $@ 


ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:%.o=.%.d)
endif


.PHONY: clean tags
.PRECIOUS: .%.d

clean:
	rm -f *.o *.elf .*.d *.bin

tags:
	ctags $$(find . \( -name *.h -o -name *.cpp -o -name *.c \) -not -name xmc4* -not -name *_cm[0-35-9]*)
