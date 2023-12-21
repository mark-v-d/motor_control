#ifndef UDP_SYNC_H
#define UDP_SYNC_H

#include "ethernet.h"
#include "drive_packet.h"

using namespace std::chrono_literals;

class udp_sync:public Ethernet::Transmitter, public Ethernet::Receiver {
    // static constexpr float kP=50e10;
    // static constexpr float kI=15e8;
    static constexpr float kP=50e8;
    static constexpr float kI=15e6;
    uint32_t addend;
    float integrator;
    std::chrono::duration<float> error;
    std::chrono::duration<float> sync_integrator;

    int unlocked;
    class __attribute__ ((__packed__)) sync_t:
	public udp_t, public sync_ns::to_drive {
    };
    sync_t pkt;
    Ethernet::timestamp_t last;
    using duration=std::chrono::duration<float>;
public:
    udp_sync(void) {}
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    duration last_error() const { return error; }

    void transmit(Ethernet *eth);
    void TimestampInit(void);
    bool locked(Ethernet *eth) {
	auto dt=eth->system_time()-last;
	if(unlocked || dt>2ms)
	    return 0;
	else
	    return 1;
    }

    duration sync(Ethernet *eth, duration limit, float Kp, float Ki) {
	auto now=eth->system_time();
	if(now-last>2ms) {
	    unlocked=100;
	    integrator=0;
	    return 0ns;
	}

	error=eth->target_time()-now;
	itm.PORT[8].f=error/1s;
	auto t(Kp*error+sync_integrator);
	sync_integrator+=Ki*error;
	auto old_t=t;
	t=std::min(limit,std::max(-limit,t));
	sync_integrator+=(t-old_t)*Ki/Kp;
	itm.PORT[10].f=sync_integrator/1s;
	if(unlocked && (error<1us || error>-1us))
	    unlocked--;
	return t;
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
