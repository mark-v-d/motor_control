#include "gpio.h"

volatile int stop;
gpio::output<0,0> DAC;
extern "C" void SysTick_Handler(void)
{
    DAC.toggle();
}

int main(int argc, char **argv)
{
    DAC.set(gpio::OUTPUT);

    SysTick_Config(10000);
    //NVIC_SetPriorityGrouping(0);
    //NVIC_SetPriority(SysTick_IRQn,0);
    NVIC_EnableIRQ(SysTick_IRQn);

    for(;;) {
    }
    return 0;
}
