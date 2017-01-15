#include "ethernet.h"

class udp_sync:public Ethernet::Transmitter, public Ethernet::Receiver {
    struct __attribute__ ((__packed__)) sync_t:public udp_t {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	uint32_t timer;

	sync_t operator =(udp_t const &o) { udp_t::operator=(o); }
    } pkt;
public:
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    udp_sync(void) { pkt.length=0; }
    void transmit(Ethernet *eth);
};
