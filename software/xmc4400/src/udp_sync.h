#ifndef UDP_SYNC_H
#define UDP_SYNC_H

#include "ethernet.h"
#include "drive_packet.h"

class udp_sync:public Ethernet::Transmitter, public Ethernet::Receiver {
    static constexpr float kP=5e10;
    static constexpr float kI=1e7;
    uint32_t addend;
    float integrator;

    using sync_t=sync_ns::to_drive;
    sync_t pkt;
    uint32_t last_s;
    uint32_t last_ns;
public:
    udp_sync(void) {}
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    void transmit(Ethernet *eth);
    void TimestampInit(void);
    bool locked(Ethernet *eth) {
	auto [now_s, now_ns]=eth->system_time();
	uint32_t dt_s=now_s-last_s;
	if(dt_s>1)
	    return 0;
	int32_t dt_ns=now_ns-last_ns;
	if(dt_ns<0)
	    dt_ns+=1'000'000'000;
	return dt_ns<1'000'000;	// 1ms timeout
    }
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
