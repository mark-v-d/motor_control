#include "icmp.h"
#include <algorithm>

using namespace std;

void icmpProcessing::Received(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
    /*pkt=desc.buffer->icmp_echo;
    swap(pkt.dst_mac,pkt.src_mac);
    swap(pkt.dst_ip,pkt.src_ip); 
    pkt.type=0; // ICMP_ECHO_REPLY;
    pkt.checksum=0;
    eth->transmit(this,&pkt,desc.length-4);
    */
}

void icmpProcessing::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}
