#include "ethernet.h"

class udp_logger:public Ethernet::Transmitter, public Ethernet::Receiver {
    struct __attribute__ ((__packed__)) logger_t:public udp_t {
	uint32_t counter;
	float adc[4];
	uint32_t encoder;
	float Irotor[2];
	float Vrotor[2];
	float angle;
	float output[3];
	uint8_t enc_raw[9];

	logger_t operator =(udp_t const &o) { udp_t::operator=(o); }
    } pkt;
public:
    virtual void Transmitted(Ethernet*,Ethernet::descriptor const&);
    virtual void Received(Ethernet*,Ethernet::descriptor const&);
    virtual void Unreachable(Ethernet*);

    udp_logger(void) { pkt.length=0; }
    void SetEncoder(uint32_t i, float a) { pkt.encoder=i; pkt.angle=a; }
    void SetADC(float a, float b, float c, float d)
    {
	pkt.adc[0]=a;
	pkt.adc[1]=b;
	pkt.adc[2]=c;
	pkt.adc[3]=d;
    }
    void SetRotor(float V[2], float I[2])
    {
	pkt.Vrotor[0]=V[0];
	pkt.Vrotor[1]=V[1];
	pkt.Irotor[0]=I[0];
	pkt.Irotor[1]=I[1];
    }
    void SetOutput(float output[3])
    {
	for(int i=0;i<3;i++)
	    pkt.output[i]=output[i];
    }
    void EncoderPacket(uint8_t *p) { memcpy(pkt.enc_raw,p,sizeof(pkt.enc_raw));}
    void transmit(Ethernet *eth);
};


void udp_logger::Received(
    Ethernet*eth,
    Ethernet::descriptor const &desc
) {
    eth->erase_udp_transmitter(this, pkt.dst_port);
    pkt=desc.buffer->udp;			// copy everything
    std::swap(pkt.dst_mac,pkt.src_mac);
    std::swap(pkt.dst_ip,pkt.src_ip); 
    pkt.dst_port=desc.buffer->udp.src_port;
    pkt.src_port=desc.buffer->udp.dst_port;
    pkt.length=hton(sizeof(pkt));
    eth->add_udp_transmitter(this, pkt.dst_port);
}

void udp_logger::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}

inline void udp_logger::transmit(Ethernet *eth)
{
    pkt.ipv4_checksum=0;	// Allow ethernet MAC to fill these
    pkt.checksum=0;
    if(pkt.length) {
	pkt.counter++;
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
