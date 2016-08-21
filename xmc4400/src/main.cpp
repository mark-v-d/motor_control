#include "XMC4500.h"
#include "gpio.h"
#include "ethernet.h"
#include "icmp.h"

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

void initSysTick(void)
{
    // Use processor clock, enable interrupt request and enable counter
    PPB->SYST_CSR |= 0x7;
    // Set the reload register (timebase in effect)
    // generate 1 millisecond time base
    PPB->SYST_RVR = SystemCoreClock / 1000 - 1; 
    PPB->SYST_CVR = 5;          // Start the counter at a value close to zero
    __enable_irq();
    PORT1->OUT = 1;
}


iopin::input<1,14> BUTTON1;
iopin::output<1,0> LED0;
iopin::output<1,1> LED1;
iopin::input<15,8> CLK_RMII;
iopin::input<15,9> CRS_DV;
iopin::input<2,2> RXD0;
iopin::input<2,3> RXD1;
iopin::input<2,4> RXER;
iopin::ETH0_MDC<2,7> MDC;
iopin::ETH0_MDO<2,0> MDIO;
iopin::ETH0_TXD0<2,8> TXD0;
iopin::ETH0_TXD1<2,9> TXD1;
iopin::ETH0_TX_EN<2,5> TX_EN;

Ethernet eth0(RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO);

uint8_t buffer[XMC_ETH_MAC_BUF_SIZE];

int main()
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    initSysTick();

    while (1) {
        /* Flash leds faster when BTN1 is pressed */
        int len=eth0.Receive(buffer,sizeof(buffer));
	packet::icmp *p=reinterpret_cast<packet::icmp *>(buffer);
        if(len>0) {
	    if(p->isEchoRequest()) {
		p->type=0;
		p->swap();
		eth0.Transmit(buffer,len);
	    }
	}

        if (tickflag) {
            tickflag = 0;
            /* Toggle leds */
            LED0^=1;
            LED1^=1;
        };

    };

    return 0;
}

extern "C" {
    void _exit(int) {}
    void _kill(int) {}
    int _getpid() {}
    void _sbrk() {}
}
