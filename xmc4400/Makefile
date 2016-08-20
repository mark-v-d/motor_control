OBJS=main.o startup_XMC4500.o System_XMC4500.o

LDSCRIPT=./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/XMC4500_series/Source/GCC/XMC4500x1024.ld

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src
VPATH+=XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/XMC4500_series/Source/GCC/


CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/XMC4500_series/Include/
CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/XMCLib/inc/
CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Include/
CPPFLAGS+=-I src
CPPFLAGS+=-DCLOCK_CRYSTAL_FREQUENCY=12000000
CPPFLAGS+=-D XMC4500_F100x1024

CPU = -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU) -O -std=c++11
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU)

.%.d: %.c Makefile
	$(CC) $(CPPFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.s
	touch $@ 

.%.d: %.S
	touch $@ 

%.elf: $(LDSCRIPT) symbols.ld
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld -o $@

%.pre: %.cpp
	$(CPP) $(CPPFLAGS) $< > $@

main.elf: $(OBJS) symbols.ld
	$(CXX) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld $+ -o $@

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:%.o=.%.d)
endif


.PHONY: clean tags
.PRECIOUS: .%.d

clean:
	rm -f *.o *.elf .*.d

tags:
	ctags $$(find . -name *.h -o -name *.cpp -o -name *.c -a -not -name xmc1*)
