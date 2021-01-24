#include "gpio.h"
#include "uart.h"

gpio::output<0,0> DAC;
gpio::pin<0,14> RXD;
gpio::pin<0,15> TXD;

auto serial=uart::make_full_duplex_no_int(TXD,RXD);

extern "C" void SysTick_Handler(void)
{
    static uint8_t i;
	serial.tx(i++);
    DAC.toggle();
}

int main(int argc, char **argv)
{
    DAC.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    SysTick_Config(1000000);
    //NVIC_SetPriorityGrouping(0);
    //NVIC_SetPriority(SysTick_IRQn,0);
    NVIC_EnableIRQ(SysTick_IRQn);

    serial.init(57600);

    for(;;) {
    }
    return 0;
}

/*
XMC_VADC_GLOBAL_BackgndAddMultipleChannels
UC_SERIES
*/
