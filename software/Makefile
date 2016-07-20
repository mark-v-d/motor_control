OBJS=main.o system_stm32f30x.o startup_stm32f334x8.o
OBJS+=stm32f30x_rcc.o

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)as
CXX=$(CROSS)g++

VPATH=src
VPATH+=CMSIS-3.2.0/Device/ST/STM32F30x/Source
VPATH+=STM32F30x_StdPeriph_Driver-V1.2.1/src


CPPFLAGS+=-I CMSIS-3.2.0/Device/ST/STM32F30x/Include/
CPPFLAGS+=-I STM32F30x_StdPeriph_Driver-V1.2.1/inc
CPPFLAGS+=-I src
CPPFLAGS+=-I CMSIS-3.2.0/Include/
CPPFLAGS += -DSTM32F334x8

CPU = -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   += $(CPU)
CXXFLAGS += $(CPU)
ASFLAGS  += $(CPU)
LDFLAGS  += $(CPU)

.%.d: %.c Makefile
	$(CC) $(CPPFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.cpp Makefile
	$(CXX) $(CPPFLAGS) -MM $<|sed -e "s/:/ $@:/" > $@ || (rm $@; false)

.%.d: %.s
	touch $@ 

%.elf: stm32f334.ld 
	$(CC) $(LDFLAGS) -T $+ -o $@

main.elf: $(OBJS)
	$(CC) $(LDFLAGS) -T stm32f334.ld $+ -o $@

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:%.o=.%.d)
endif


.PHONY: clean
.PRECIOUS: .%.d

clean:
	rm -f *.o *.elf .*.d
