#include "XMC4500.h"
#include "gpio.h"
#include "ethernet.h"
#include "icmp.h"
#include <atomic>

std::atomic<int> counter(0);

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

extern "C" void SysTick_Handler(void)
{
    counter++;
}

icmpProcessing icmp;
Ethernet eth0(
    0x000319450000ULL, 0, 
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);

int main()
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    SysTick_Config(SystemCoreClock/1000);


    while (1) {
        if(counter>500) {
            counter-=500;
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
    void _read() {}
    void _close() {}
    void _fstat() {}
    void _lseek() {}
    void _isatty() {}
    void _write() {}
    void _open() {}
}
