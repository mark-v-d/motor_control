#include "udp_sync.h" 
#include "hardware.h"
#include "ccu8.h"
#include "pwm_3phase.h"

void udp_sync::Received(Ethernet*eth, Ethernet::descriptor const &desc)
{
    sync_t *p=reinterpret_cast<sync_t*>(desc.buffer);
    pkt=*p;


    // Set packet source
    eth->set_saddr(&pkt);
    pkt.src_port=desc.buffer->udp.dst_port;


    // Set packet destination
    memcpy(pkt.dst_mac,p->src_mac,sizeof(pkt.dst_mac));
    memcpy(pkt.dst_ip,p->src_ip,sizeof(pkt.dst_ip));
    pkt.dst_port=p->src_port;
    pkt.type=p->type;

    pkt.length=hton(sizeof(pkt));

    pkt.tx_seconds=p->tx_seconds;
    pkt.tx_nanoseconds=p->tx_nanoseconds; 
    pkt.rx_seconds=desc.seconds;
    pkt.rx_nanoseconds=desc.nanoseconds;
    pkt.timer=ccu8[ccu8_ns::unit(HB0)].cc[spare_slice(HB0,HB1,HB2)].TIMER;
    transmit(eth);
}

void udp_sync::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}

inline void udp_sync::transmit(Ethernet *eth)
{
    pkt.ipv4_checksum=0;	// Allow ethernet MAC to fill these
    pkt.checksum=0;
    if(pkt.length) {
	pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
	pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
	eth->transmit(this,&pkt,sizeof(pkt));
    }
}

void udp_sync::Unreachable(Ethernet *eth)
{
    eth->erase_udp_transmitter(this,pkt.dst_port);
    pkt.length=0;
}
