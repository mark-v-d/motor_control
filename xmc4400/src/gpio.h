#ifndef GPIO_H
#define GPIO_H

#include "xmc_gpio.h"
#include "xmc_eth_mac.h"

struct XMC_GPIO_PORT_padded:public XMC_GPIO_PORT
{
  __I  uint32_t  RESERVED5[34];
};
extern XMC_GPIO_PORT_padded gpio_port[15];

template <int port, int pin>
class iopin
{
public:
    int operator=(int i) { gpio_port[port].OMR=(i? 1:0x10000)<<pin; return i; }
    void operator^=(int i) { if(i) gpio_port[port].OMR=0x10001<<pin; }
    operator int(void) { return (gpio_port[port].IN>>pin)&1; }

    void set(XMC_GPIO_MODE m) { 
	// Isn't this byte adressable?
	gpio_port[port].IOCR[pin/4]&=~(255<<8*(pin%4)); 
	gpio_port[port].IOCR[pin/4]|=m<<8*(pin%4);
    }
    void set(XMC_GPIO_OUTPUT_STRENGTH m) { 
	gpio_port[port].PDR[pin/8]&=~(15<<4*(pin%8));
	gpio_port[port].PDR[pin/8]|=m<<4*(pin%8);
    }
    void pdisc(int i) {
	if(i)
	    gpio_port[port].PDISC|=1<<pin;
	else
	    gpio_port[port].PDISC&=~(1<<pin);
    }
    void pps(int i) {
	if(i)
	    gpio_port[port].PPS|=1<<pin;
	else
	    gpio_port[port].PPS&=~(1<<pin);
    }
    void hwsel(int i) {
	gpio_port[port].HWSEL&=~(3<<2*pin);
	gpio_port[port].HWSEL|=(i&3)<<2*pin;
    }
    operator XMC_ETH_MAC_PORT_CTRL_RXD0() {
	static_assert(port==2 && pin==2,
	    "Cannot use this pin as RXD0 for ETH0");
    }
};

template <>
iopin<2,2>::operator XMC_ETH_MAC_PORT_CTRL_RXD0()
{
    return XMC_ETH_MAC_PORT_CTRL_RXD0_P2_2;
};

template <>
iopin<0,2>::operator XMC_ETH_MAC_PORT_CTRL_RXD0()
{
    return XMC_ETH_MAC_PORT_CTRL_RXD0_P0_2;
};

template <>
iopin<14,8>::operator XMC_ETH_MAC_PORT_CTRL_RXD0()
{
    return XMC_ETH_MAC_PORT_CTRL_RXD0_P14_8;
};

template <>
iopin<5,0>::operator XMC_ETH_MAC_PORT_CTRL_RXD0()
{
    return XMC_ETH_MAC_PORT_CTRL_RXD0_P5_0;
};


void ftest(XMC_ETH_MAC_PORT_CTRL_RXD0);
#endif
