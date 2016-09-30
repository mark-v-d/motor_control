#include <atomic>

#include "gpio.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu8.h"
#include "pwm_3phase.h"

std::atomic<int> counter(0);

#if (UC_DEVICE == XMC4500)
iopin::input<1,14> BUTTON1;
iopin::output<1,0> LED0;
iopin::output<1,1> LED1;
iopin::output<1,2> LED2;
iopin::output<1,3> LED3;
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
#else
iopin::output<0,7> LED0=1;
iopin::output<0,11> LED1=1;
iopin::output<0,4> LED2=1;
iopin::output<0,2> LED3=1;
iopin::output<0,9> LED4=1;
iopin::input<2,1> CLK_RMII;
iopin::input<0,1> CRS_DV;
iopin::input<2,2> RXD0;
iopin::input<2,3> RXD1;
iopin::input<2,4> RXER;
iopin::ETH0_MDC<2,7> MDC;
iopin::ETH0_MDO<2,0> MDIO;
iopin::ETH0_TXD0<2,8> TXD0;
iopin::ETH0_TXD1<2,9> TXD1;
iopin::ETH0_TX_EN<2,5> TX_EN;
iopin::output<1,0> ETH_RESET=1;

iopin::U0C0_DOUT0<1,5> ENC_TXD;

iopin::CCU80_OUT20<0,3> HB1;
iopin::CCU80_OUT00<0,5> HB0;
iopin::CCU80_OUT30<0,6> HB2;
#endif

extern "C" void SysTick_Handler(void)
{
    counter++;
}

extern "C" void CCU80_1_IRQHandler(void)
{
    LED2=0;
    counter++;
    LED2=1;
}

icmpProcessing icmp;
Ethernet eth0(
    0x000319450000ULL, 0, 
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);


extern ETH_GLOBAL_TypeDef eth_debug;
int main()
{

    SysTick_Config(SystemCoreClock/1000);

    pwm_3phase(HB0,HB1,HB2,18000);

    HB1=100;

    int t=0;
    while (1) {
        if(counter>500) {
            counter-=500;
            LED1^=1;
            HB0=t&1? 2270:2000;
            HB2=t&1? 1500:2000;
            t++;
	    XMC_CCU8_EnableShadowTransfer(HB0, 0x1111);
	    LED3=eth_debug.RX_FRAMES_COUNT_GOOD_BAD&1;
        };
    };

    return 0;
}


/* This removes a lot of code from the binary
*/
void __gnu_cxx::__verbose_terminate_handler(void)
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED2.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED3.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    for(;;) {
	LED0^=1;
	LED1^=1;
	LED2^=1;
    }
}
