#ifndef BSL_H
#define BSL_H

#include "uart.h"

extern char xmc1300_start[], xmc1300_end[];

template <class TX_PIN, class RX_PIN>
void bsl_init(TX_PIN const&,RX_PIN const&) {
    using namespace std::chrono_literals;
    auto copro=uart::make_full_duplex_no_int(TX_PIN{},RX_PIN{});
    copro.init(57600); // This seems to be the max
    main_sleep(10ms);

    uint32_t rx_data;
    do {
	copro.tx(0);
	copro.tx(0x6c);
    } while(copro.rx(2ms)!=0x5d);

    int length=xmc1300_end-xmc1300_start;
    do {
	copro.tx((length>> 0)&255);
	copro.tx((length>> 8)&255);
	copro.tx((length>>16)&255);
	copro.tx((length>>24)&255);
    } while(copro.rx(2ms)!=0x01);

    do {
	char *p=xmc1300_start;
	while(p<xmc1300_end)
	    copro.tx(*p++);
    } while(copro.rx(2ms)!=0x01);
}

#endif
