#include "ethernet.h"

class udp_poker:public Ethernet::Receiver {
    struct pkt_t {
	uint32_t hb[3];

	pkt_t operator =(udp_payload_t const &o) {
	    memcpy(hb,o.data,sizeof(hb));
	}
    } pkt;
public:
    virtual void Received(Ethernet*,Ethernet::descriptor const&);

    udp_poker(void) { }
    uint32_t hb0(void) { return pkt.hb[0]; }
    uint32_t hb1(void) { return pkt.hb[1]; }
    uint32_t hb2(void) { return pkt.hb[2]; }
};


void udp_poker::Received(
    Ethernet*eth,
    Ethernet::descriptor const &desc
) {
    pkt=desc.buffer->udp_payload;
}
