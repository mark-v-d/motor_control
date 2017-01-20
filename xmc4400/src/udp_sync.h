#ifndef UDP_SYNC_H
#define UDP_SYNC_H

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

inline void udp_sync::transmit(Ethernet *eth)
{
    pkt.checksum=0;
    pkt.version_headerlength=0x45;	// IPv4
    pkt.services=0;
    pkt.flags_fragment_offset=0x40;	// Don't fragment
    pkt.ttl=255;
    pkt.protocol=ipv4_header_t::UDP;
    pkt.ipv4_checksum=0;	// Allow ethernet MAC to fill these
    pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
    pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
    eth->transmit(this,&pkt,sizeof(pkt));
}

#endif
