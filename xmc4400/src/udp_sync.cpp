#include "udp_sync.h" 
#include "hardware.h"
#include "ccu8.h"
#include "pwm_3phase.h"
#include "bitfields.h"

void udp_sync::TimestampInit(void)
{
    using namespace eth_ns;
    // ETH_GLOBAL_TypeDef
    eth.SYSTEM_TIME_NANOSECONDS_UPDATE=100;
    eth.SYSTEM_TIME_SECONDS_UPDATE=11;
    timestamp_control_t control({{
	.tsena=1,	// enable timestamp
	.tscfupdt=0,	// Coarse update
	.tsinit=1,	// Try to init 
	.tsupdt=0,	// Nothing to update
	.tstrig=0,	// No interrupt
	.tsaddreg=0,	// Don't update addend

	.tsenall=1,	// Timestamp on all frames
	.tsctrlssr=1,	// digital rollover (1ns resolution)
	.tsver2ena=1,	// FIXME, not sure what difference this makes
	.tsipena=0,	// No PTP over raw ethernet
	.tsipv6ena=0,	// No ipv6
	.tsipv4ena=1,	// Enable ipv4 processing
	.tsevntena=0,	// Only event messages
	.tsmstrena=0,	// Only master messages
	.snaptypsel=0,	// FIXME, what does this button do?
	.tsenmacaddr=0	// No mac filtering
    }});
    eth.TIMESTAMP_CONTROL=control.raw;
    uint32_t clk=XMC_SCU_CLOCK_GetSystemClockFrequency();
    uint32_t ssi=uint32_t(floorf(1.0e9F/clk))+1;
    addend=roundf(1.0e9F*powf(2,32)/(ssi*clk));
    eth.SUB_SECOND_INCREMENT=ssi;
    do {
	control.raw=eth.TIMESTAMP_CONTROL;
    } while(control.tsinit);
    control.tscfupdt=1;
    control.tsaddreg=1;
    eth.TIMESTAMP_ADDEND=addend;
    eth.TIMESTAMP_CONTROL=control.raw;
}

udp_sync::udp_sync(void)
{
    TimestampInit();
}

void udp_sync::Received(Ethernet *ep, Ethernet::descriptor const &desc)
{
    sync_t *p=reinterpret_cast<sync_t*>(desc.buffer);
    pkt=*p;

    // Set packet source
    ep->set_saddr(&pkt);
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

    eth_ns::timestamp_control_t control;
    control.raw=eth.TIMESTAMP_CONTROL;
    if(!(control.tsinit | control.tsupdt | control.tsaddreg)) {
	int32_t d_sec=pkt.tx_seconds-pkt.rx_seconds;
	int32_t d_nsec=(pkt.tx_nanoseconds>pkt.rx_nanoseconds)?
	    pkt.tx_nanoseconds-pkt.rx_nanoseconds
	    :-int32_t(pkt.rx_nanoseconds-pkt.tx_nanoseconds);
	float error=d_sec+1e-9F*d_nsec;
	if(fabs(error)>1e-3) {
	    eth.SYSTEM_TIME_SECONDS_UPDATE=error;
	    eth.SYSTEM_TIME_NANOSECONDS_UPDATE=fmodf(error,1.0F)*1e9;
	    control.tsupdt=1;
	    integrator=0;
	} else {
	    integrator+=error*kI;
	    eth.TIMESTAMP_ADDEND=addend+integrator+kP*error;
	    control.tsaddreg=1;
	}
	eth.TIMESTAMP_CONTROL=control.raw;
    }
    pkt.integrator=integrator;

    transmit(ep);
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
    pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
    pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
    eth->transmit(this,&pkt,sizeof(pkt));
}

void udp_sync::Unreachable(Ethernet *eth)
{
    eth->erase_udp_transmitter(this,pkt.dst_port);
    pkt.length=0;
}
