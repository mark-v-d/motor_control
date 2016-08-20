#include "XMC4500.h"
#include "gpio.h"

/****************************************************
 * 
 * System Timer
 * 
 * **************************************************/

volatile uint32_t tickflag;
volatile uint32_t systick_count;

#ifdef __cplusplus
extern "C"
#endif
void SysTick_Handler(void)
{
    systick_count++;
    if(!(systick_count % 500 ))
	tickflag = 1;
}

void initSysTick()
{
    // Use processor clock, enable interrupt request and enable counter
    PPB->SYST_CSR |= 0x7;
    // Set the reload register (timebase in effect)
    PPB->SYST_RVR = SystemCoreClock / 1000 - 1; //generate 1 millisecond time base
    PPB->SYST_CVR = 5;          // Start the counter at a value close to zero
    __enable_irq();
    PORT1->OUT = 1;
}


#if 0
iopin<0> &LED0=*reinterpret_cast<iopin<0>*>(XMC_GPIO_PORT1);
iopin<1> &LED1=*reinterpret_cast<iopin<1>*>(XMC_GPIO_PORT1);
iopin<14> &BUTTON1=*reinterpret_cast<iopin<14>*>(XMC_GPIO_PORT1);
#else
iopin<1,0> LED0;
iopin<1,1> LED1;
iopin<1,14> BUTTON1;
iopin<2,2> RXD0;
#endif

int main()
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    RXD0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    initSysTick();

    ftest(LED0);

    while (1) {
        /* Flash leds faster when BTN1 is pressed */
        if (tickflag) {
            tickflag = 0;
            /* Toggle leds */
            LED0^=1;
            LED1^=1;
        };

    };

    return 0;
}

extern "C" void _exit(int) {}
