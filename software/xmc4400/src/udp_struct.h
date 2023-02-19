#ifndef UDP_STRUCT_H
#define UDP_STRUCT_H
#include "ethernet.h"

template <typename rx_t, typename tx_t>
class udp_struct:public Ethernet::Transmitter, public Ethernet::Receiver {
    struct  __attribute__ ((__packed__)) pkt_in_t:public udp_t {
	rx_t input;
    };

    struct  __attribute__ ((__packed__)) pkt_out_t:public udp_t {
	tx_t output;
    } pkt;

    rx_t data;
    uint32_t seconds, nanoseconds;
public:
    rx_t *operator->() { return &data; }

    void Received(
	Ethernet *eth,
	Ethernet::descriptor const &desc
    ) override {
	pkt_in_t *p=reinterpret_cast<pkt_in_t*>(desc.buffer);
	seconds=desc.seconds;
	nanoseconds=desc.nanoseconds;

	eth->set_ipv4_address(p->dst_ip);

	// Set packet source
	eth->set_saddr(&pkt);
	pkt.src_port=desc.buffer->udp.dst_port;

	// Set packet destination
	pkt.dst_mac=p->src_mac;
	pkt.dst_ip=p->src_ip;
	pkt.dst_port=p->src_port;
	pkt.type=p->type;

	pkt.length=hton(sizeof(pkt));
	pkt.version_headerlength=0x45;
	pkt.services=0;
	pkt.id=p->id;
	pkt.flags_fragment_offset=p->flags_fragment_offset;
	pkt.ttl=p->ttl;
	pkt.protocol=ipv4_header_t::UDP;

	data=p->input;
    }

    void Transmitted(
	Ethernet *eth,
	Ethernet::descriptor const &desc
    ) {
    }

    void transmit(Ethernet *eth, tx_t const &d)
    {
	pkt.ipv4_checksum=0;	// Allow ethernet MAC to fill these
	pkt.checksum=0;
	if(pkt.length) {
	    pkt.output=d;
	    pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
	    pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
	    eth->transmit(this,&pkt,sizeof(pkt));
	}
    }

    void Unreachable(Ethernet *eth)
    {
	eth->erase_udp_transmitter(this,pkt.dst_port);
	pkt.length=0;
    }

    std::chrono::duration<float> age(Ethernet *eth) {
	using namespace std::chrono_literals;
	auto [now, now_ns]=eth->system_time();
	float dt=1e-9f*int32_t(now_ns-nanoseconds);
	dt+=now-seconds;
	return dt*1s;
    }
};

#endif
