#ifndef ICMP_H
#define ICMP_H

#include "ethernet.h"

class icmpProcessing:public Ethernet::Receiver, public Ethernet::Transmitter {
    static icmp_echo_t pkt;
public:
    void Received(Ethernet*,Ethernet::descriptor const&);
    void Transmitted(Ethernet*,Ethernet::descriptor const&);
    void Unreachable(Ethernet*) {}
};

#endif
