#include "ethernet.h"

class udp_sync:public Ethernet::Transmitter, public Ethernet::Receiver {
    float kP=5e10;
    float kI=1e7;
    uint32_t addend;
    float integrator;

    struct __attribute__ ((__packed__)) sync_t:public udp_t {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	uint32_t timer;
	float integrator;

	sync_t operator =(udp_t const &o) { udp_t::operator=(o); }
    } pkt;
public:
    udp_sync(void);
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    void transmit(Ethernet *eth);
private:
    void TimestampInit(void);
};
