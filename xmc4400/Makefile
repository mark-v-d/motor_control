DEVICE=XMC4400
ifeq ($(DEVICE),XMC4400)
	PACKAGE=F64
	MEMORY=512
else
	PACKAGE=F100
	MEMORY=1024
endif

OBJS=main.o startup_$(DEVICE).o system_$(DEVICE).o ethernet.o icmp.o system.o xmc4_scu.o
# xmc_eth_mac.o 
# xmc_eth_phy_ksz8031rnl.o

#LDSCRIPT=./$(DEVICE)x$(MEMORY).ld
LDSCRIPT=./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/$(DEVICE)_series/Source/GCC/$(DEVICE)x$(MEMORY).ld


CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src
VPATH+=XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/$(DEVICE)_series/Source/GCC/
VPATH+=XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/$(DEVICE)_series/Source/
VPATH+=XMC_Peripheral_Library_v2.1.6p1/XMCLib/src/


CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Infineon/$(DEVICE)_series/Include/
CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/XMCLib/inc/
CPPFLAGS+=-I ./XMC_Peripheral_Library_v2.1.6p1/CMSIS/Include/
CPPFLAGS+=-I src
CPPFLAGS+=-DCLOCK_CRYSTAL_FREQUENCY=12000000
CPPFLAGS+=-D $(DEVICE)_$(PACKAGE)x$(MEMORY)
#CPPFLAGS+=-D UC_ID -D OSCHP_FREQUENCY=12000000

CPU = -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU) -std=c++11
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU)

%.o: Makefile

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
	ctags $$(find . \( -name *.h -o -name *.cpp -o -name *.c \) -not -name xmc1* -not -name *_cm[0-35-9]*)

src/gpio_output_conversions: ./XMC_Peripheral_Library_v2.1.6p1/XMCLib/inc/xmc4_gpio_map.h tools/convert_header
	./tools/convert_header < $< > $@

tools/convert_header: tools/convert_header.cpp
	make -C tools convert_header
