#include "icmp.h"
#include <algorithm>

using namespace std;
icmp_echo_t icmpProcessing::pkt __attribute__((section ("ETH_RAM")));

void icmpProcessing::Received(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
    pkt=desc.buffer->icmp_echo;
    swap(pkt.dst_mac,pkt.src_mac);
    swap(pkt.dst_ip,pkt.src_ip); 
    pkt.type=0; // ICMP_ECHO_REPLY;
    pkt.ipv4_checksum=0;
    pkt.icmp_checksum=0;
    eth->transmit(this,&pkt,(0x3fff&(desc.status>>16))-4);
}

void icmpProcessing::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}
