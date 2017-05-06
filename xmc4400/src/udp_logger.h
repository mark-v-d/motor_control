#include "ethernet.h"

class udp_logger:public Ethernet::Transmitter, public Ethernet::Receiver {
public:
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    struct  __attribute__ ((__packed__)) output_t {
	uint32_t counter;
	int32_t position;
	float adc[4];
	float Irotor[2];
	float Vrotor[2];
	float I[2];
	float angle;
	float output[3];
	float vservo;
	uint16_t index2;
	uint16_t position2;
    };

    struct  __attribute__ ((__packed__)) input_t {
	float Iset[2];
	float kP[2], kI[2];
	float lim;
    };

    udp_logger(struct input_t*);
    void transmit(Ethernet *eth, output_t const &d);


private:
    struct  __attribute__ ((__packed__)) pkt_in_t:public udp_t {
	input_t input;
    };

    struct  __attribute__ ((__packed__)) pkt_out_t:public udp_t {
	output_t output;
    } pkt;

    struct input_t *data;
};

udp_logger::udp_logger(struct input_t *rx)
{
    data=rx;
}

void udp_logger::Received(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
    pkt_in_t *p=reinterpret_cast<pkt_in_t*>(desc.buffer);

    eth->set_ipv4_address(p->dst_ip); 

    // Set packet source
    eth->set_saddr(&pkt);
    pkt.src_port=desc.buffer->udp.dst_port;

    // Set packet destination
    memcpy(pkt.dst_mac,p->src_mac,sizeof(pkt.dst_mac));
    memcpy(pkt.dst_ip,p->src_ip,sizeof(pkt.dst_ip));
    pkt.dst_port=p->src_port;
    pkt.type=p->type;

    pkt.length=hton(sizeof(pkt));
    pkt.version_headerlength=0x45;
    pkt.services=0;
    pkt.id=p->id;
    pkt.flags_fragment_offset=p->flags_fragment_offset;
    pkt.ttl=p->ttl;
    pkt.protocol=ipv4_header_t::UDP;

    if(data)
	memcpy(data,&p->input,sizeof(p->input));
}

void udp_logger::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}

inline void udp_logger::transmit(Ethernet *eth, output_t const &d)
{
    pkt.ipv4_checksum=0;	// Allow ethernet MAC to fill these
    pkt.checksum=0;
    if(pkt.length) {
	memcpy(&pkt.output,&d,sizeof(d));
	pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
	pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
	eth->transmit(this,&pkt,sizeof(pkt));
    }
}

void udp_logger::Unreachable(Ethernet *eth)
{
    eth->erase_udp_transmitter(this,pkt.dst_port);
    pkt.length=0;
}
