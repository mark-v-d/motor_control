#ifndef GPIO_H
#define GPIO_H

#include "xmc_gpio.h"

template <int pin>
class iopin:public XMC_GPIO_PORT
{
    public:

    int operator=(int i) { OMR=(i? 1:0x10000)<<pin; return i; }
    void operator^=(int i) { if(i) OMR=0x10001<<pin; }
    operator int(void) { return (IN>>pin)&1; }

    void set(XMC_GPIO_MODE m) { 
	// Isn't this byte adressable?
	IOCR[pin/4]&=~(255<<8*(pin%4)); 
	IOCR[pin/4]|=m<<8*(pin%4);
    }
    void set(XMC_GPIO_OUTPUT_STRENGTH m) { 
	PDR[pin/8]&=~(15<<4*(pin%8));
	PDR[pin/8]|=m<<4*(pin%8);
    }
    void pdisc(int i) {
	if(i)
	    PDISC|=1<<pin;
	else
	    PDISC&=~(1<<pin);
    }
    void pps(int i) {
	if(i)
	    PPS|=1<<pin;
	else
	    PPS&=~(1<<pin);
    }
    void hwsel(int i) {
	HWSEL&=~(3<<2*pin);
	HWSEL|=(i&3)<<2*pin;
    }
};

#endif
