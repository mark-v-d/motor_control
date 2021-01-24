#include "gpio.h"
#include "uart.h"

gpio::output<0,0> DAC;
gpio::pin<0,14> RXD;
gpio::pin<0,15> TXD;

extern "C" void SysTick_Handler(void)
{
    DAC.toggle();
}

int main(int argc, char **argv)
{
    DAC.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    SysTick_Config(10000);
    //NVIC_SetPriorityGrouping(0);
    //NVIC_SetPriority(SysTick_IRQn,0);
    NVIC_EnableIRQ(SysTick_IRQn);

    auto serial=uart::make_full_duplex_no_int(TXD,RXD);

    serial.init(19200);

    for(;;) {
    }
    return 0;
}
