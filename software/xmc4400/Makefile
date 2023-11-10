DEVICE=XMC4400
ifeq ($(DEVICE),XMC4400)
	PACKAGE=F100
	MEMORY=512
else
	PACKAGE=F100
	MEMORY=1024
endif

XMCLIB=../../mtb-xmclib-cat3/

OBJS=main.o startup_$(DEVICE).o system_$(DEVICE).o ethernet.o icmp.o system.o 
OBJS+=udp_sync.o
OBJS+=xmc4_scu.o xmc_ccu8.o xmc_usic.o xmc_uart.o xmc4_gpio.o encoder.o 
OBJS+=xmc_posif.o xmc_ccu4.o xmc_eru.o xmc4_eru.o xmc_vadc.o
OBJS+=xmc1300_bsl.o xmc_hrpwm.o
# xmc_eth_mac.o 
# xmc_eth_phy_ksz8031rnl.o

LDSCRIPT=./$(DEVICE)x$(MEMORY).ld
#LDSCRIPT=$(DEVICE)x$(MEMORY).ld


CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src
VPATH+=$(XMCLIB)/CMSIS/Infineon/COMPONENT_$(DEVICE)/Source/
VPATH+=$(XMCLIB)/CMSIS/Infineon/COMPONENT_$(DEVICE)/Source/TOOLCHAIN_GCC_ARM/
VPATH+=$(XMCLIB)/XMCLib/src/

CPPFLAGS+=-g -I ../include -I ./include 
CPPFLAGS+=-I $(XMCLIB)/CMSIS/Infineon/COMPONENT_$(DEVICE)/Include/
CPPFLAGS+=-I $(XMCLIB)/XMCLib/inc/
CPPFLAGS+=-I $(XMCLIB)/CMSIS/Core/Include/
CPPFLAGS+=-I src   
CPPFLAGS+=-DCLOCK_CRYSTAL_FREQUENCY=12000000
CPPFLAGS+=-DFREQUENCY=120000000
CPPFLAGS+=-D $(DEVICE)_$(PACKAGE)x$(MEMORY) 
#CPPFLAGS+=-D UC_ID -D OSCHP_FREQUENCY=12000000

CPU = -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU) -std=c++20 -O2 -Wno-volatile 
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

%.elf: $(LDSCRIPT) 
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld -o $@

%.pre: %.cpp
	$(CPP) $(CPPFLAGS) $< > $@

.PHONY: clean tags all xmc1300

all: xmc1300 main.elf 

main.elf: $(OBJS) 
	$(CXX) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld $+ -o $@

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:%.o=.%.d)
endif


.PRECIOUS: .%.d

clean:
	rm -f *.o *.elf .*.d

TAG_PATTERN=\( -name *.h -o -name *.cpp -o -name *.c \) -not -name xmc1* -not -name *_cm[0-35-9]*)
tags:
	ctags $$(find src $(TAG_PATTERN) \
		$$(find ../include $(TAG_PATTERN) \
		$$(find $(XMCLIB)/CMSIS/Core $(TAG_PATTERN) \
		$$(find $(XMCLIB)/CMSIS/Infineon/COMPONENT_$(DEVICE)/ $(TAG_PATTERN) \
		$$(find $(XMCLIB)/XMCLib $(TAG_PATTERN) 

src/gpio_output_conversions: $(XMCLIB)/XMCLib/inc/xmc4_gpio_map.h tools/convert_header
	./tools/convert_header < $< > $@

tools/convert_header: tools/convert_header.cpp
	make -C tools convert_header

set_bmi.o: set_bmi.cpp
	$(CXX) $(CXXFLAGS) -DMAC=$(MAC) -DIPV4=$(IPV4) $(CPPFLAGS) -c $<

set_bmi.elf: startup_$(DEVICE).o system_$(DEVICE).o set_bmi.o 
	$(CXX) $(LDFLAGS) -T $(LDSCRIPT) -T symbols.ld $+ -o $@

.xmc1300_bsl.d:
	touch $@

xmc1300_bsl.o: ../xmc1300/xmc1300.bin 
	echo|$(AS) $(ASFLAGS) -o $@
	$(CROSS)objcopy --add-section xmc1300=$< $@ 

xmc1300:
	$(MAKE) -C ../xmc1300 xmc1300.bin
