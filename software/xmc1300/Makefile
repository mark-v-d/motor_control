DEVICE=XMC1301_T016x0032
OBJS=main.o startup_XMC1300.o system_XMC1300.o xmc_vadc_reduced.o

XMCLIB=../../mtb-xmclib-cat3

LDSCRIPT=xmc1300.ld

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src
VPATH+=:$(XMCLIB)/XMCLib/src

CPPFLAGS+=-g -I ../include
CPPFLAGS+=-I $(XMCLIB)/CMSIS/Infineon/COMPONENT_XMC1300/Include/
CPPFLAGS+=-I $(XMCLIB)/CMSIS/Core/Include/
CPPFLAGS+=-I $(XMCLIB)/XMCLib/inc/
CPPFLAGS+=-D FREQUENCY=32000000
CPPFLAGS+=-D $(DEVICE)

CPU = -g -mthumb -mcpu=cortex-m0 -gdwarf-4

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU) -std=c++2a -O3 -Wno-volatile
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

TAG_PATTERN=\( -name *.h -o -name *.cpp -o -name *.c \) -not -name xmc4* -not -name *_cm[1-9]*)
tags:
	ctags $$(find src $(TAG_PATTERN) \
		$$(find ../include $(TAG_PATTERN) \
		$$(find $(XMCLIB)/CMSIS/Core $(TAG_PATTERN) \
		$$(find $(XMCLIB)/CMSIS/Infineon/COMPONENT_XMC1300/ $(TAG_PATTERN) \
		$$(find $(XMCLIB)/XMCLib $(TAG_PATTERN) 
