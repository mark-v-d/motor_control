#include "icmp.h"
#include <algorithm>

using namespace std;
icmp_echo_t icmpProcessing::pkt __attribute__((section ("ETH_RAM")));

void icmpProcessing::Received(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
    pkt=desc.buffer->icmp_echo;
    switch(pkt.type) {
    case icmp_t::ECHO:
	// swap(pkt.dst_mac,pkt.src_mac);

	memcpy(pkt.dst_mac,pkt.src_mac,6);
	memcpy(pkt.src_mac,g_chipid,6);
	pkt.src_mac[0]&=~1;
	pkt.src_mac[0]|=2;

	swap(pkt.dst_ip,pkt.src_ip); 
	pkt.type=0; // ICMP_ECHO_REPLY;
	pkt.ipv4_checksum=0;
	pkt.icmp_checksum=0;
	eth->transmit(this,&pkt,(0x3fff&(desc.status>>16))-4);
	break;
    case icmp_t::UNREACHABLE: {
	auto pkt=desc.buffer->icmp_unreachable_udp; 
	if(pkt.ipv4.protocol==ipv4_t::UDP)
	    eth->Unreachable_udp(pkt.udp.dst_port);
	} break;
    }
}

void icmpProcessing::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}
