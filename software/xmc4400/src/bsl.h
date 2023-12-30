#ifndef BSL_H
#define BSL_H

#include "uart.h"

extern char xmc1300_start[], xmc1300_end[];


template <class POWER_ENABLE, class TX_PIN, class RX_PIN>
void bsl_init(POWER_ENABLE &power_enable, TX_PIN const&,RX_PIN const&) {
    constexpr auto BSL_BAUDRATE=uart::Baudrate(115200);

    using namespace std::chrono_literals;
    uart::full_duplex copro(TX_PIN{},RX_PIN{});
    copro.init(BSL_BAUDRATE);

    uint32_t rx_data;

    for(;;) {
	power_enable=0;
	itm.PORT[0].u8=1;
	main_sleep(100ms);
	power_enable=1;
	itm.PORT[0].u8=2;
	main_sleep(100ms);
	for(int x=0; x<100; x++) {
	    itm.PORT[0].u8=3;
	    copro.tx(0);
	    copro.tx(0x6c);
	    if(copro.rx(2ms)==0x5d)
		goto upload;
	}
    }

upload:
    itm.PORT[0].u8=4;

    int length=xmc1300_end-xmc1300_start;
    do {
	copro.tx((length>> 0)&255);
	copro.tx((length>> 8)&255);
	copro.tx((length>>16)&255);
	copro.tx((length>>24)&255);
	itm.PORT[0].u8=5;
    } while(copro.rx(2ms)!=0x01);

    do {
	itm.PORT[0].u8=6;
	char *p=xmc1300_start;
	while(p<xmc1300_end)
	    copro.tx(*p++);
    } while(copro.rx(2ms)!=0x01);
    itm.PORT[0].u8=7;
}

#endif
