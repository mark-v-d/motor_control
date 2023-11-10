#include "udp_sync.h"
#include "hardware.h"
#include "bitfields.h"
#include <cmath>

void udp_sync::TimestampInit(void)
{
    using namespace eth_ns;
    // ETH_GLOBAL_TypeDef
    eth.SYSTEM_TIME_NANOSECONDS_UPDATE=100;
    eth.SYSTEM_TIME_SECONDS_UPDATE=11;
    timestamp_control_t control({{
	.tsena=1,	// enable timestamp
	.tscfupdt=1,	// Fine update
	.tsinit=1,	// Try to init
	.tsupdt=0,	// Nothing to update
	.tstrig=0,	// No interrupt
	.tsaddreg=1,	// Update addend

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
    uint32_t clk=XMC_SCU_CLOCK_GetSystemClockFrequency();
    uint32_t ssi=uint32_t(floorf(1.0e9F/clk))+1;
    addend=roundf(1.0e9F*powf(2,32)/(ssi*clk));
    eth.SUB_SECOND_INCREMENT=ssi;
    eth.TIMESTAMP_ADDEND=addend;
    eth.TIMESTAMP_CONTROL=control.raw;
}

void udp_sync::Received(Ethernet *p_eth, Ethernet::descriptor const &desc)
{
    sync_t *p=reinterpret_cast<sync_t*>(desc.buffer);

    eth_ns::timestamp_control_t control;
    control.raw=eth.TIMESTAMP_CONTROL;
    last={desc.seconds,desc.nanoseconds};

    // Schedule synchronisation event
    constexpr int pre_trigger=40'000; // timestamp 40us before rx packet
    if(p->rx_nanoseconds>=pre_trigger) {
	eth.TARGET_TIME_SECONDS=p->rx_seconds;
	eth.TARGET_TIME_NANOSECONDS=p->rx_nanoseconds-pre_trigger;
    } else {
	eth.TARGET_TIME_SECONDS=p->rx_seconds-1;
	eth.TARGET_TIME_NANOSECONDS=p->rx_nanoseconds+1'000'000'000-pre_trigger;
    }
    //control.tstrig=1;

    float err=0;
    if(!(control.tsinit | control.tsupdt | control.tsaddreg)) {
	// When ready update the ADDEND register
	int32_t d_sec=p->tx_seconds-desc.seconds;
	int32_t d_nsec=(p->tx_nanoseconds>desc.nanoseconds)?
	    p->tx_nanoseconds-desc.nanoseconds
	    :-int32_t(desc.nanoseconds-p->tx_nanoseconds);
	err=d_sec+1e-9F*d_nsec;
	itm.PORT[4].f=err;
	if(fabs(err)>1e-3) {
	    // If the err exceeds 1ms, reset the time
	    eth.SYSTEM_TIME_SECONDS_UPDATE=d_sec;
	    eth.SYSTEM_TIME_NANOSECONDS_UPDATE=d_nsec;
	    control.tsupdt=1;
	    integrator=0;
	} else {
	    // Usually the time is kept up-to-date using a PI controller
	    integrator+=err*kI;
	    auto t=addend+integrator+kP*err;
	    eth.TIMESTAMP_ADDEND=t;
	    itm.PORT[6].f=t;
	    control.tsaddreg=1;
	}
	itm.PORT[5].f=integrator;
    }

    eth.TIMESTAMP_CONTROL=control.raw;
    pkt.integrator=integrator;

    if(p->dst_ip[3]!=255) {
	// Respond to the sync if it was not a broadcast

	// Set packet source
	p_eth->set_saddr(&pkt);
	pkt.src_port=desc.buffer->udp.dst_port;

	// Set packet destination
	pkt.dst_mac=p->src_mac;
	pkt.dst_ip=p->src_ip;
	pkt.dst_port=p->src_port;
	pkt.type=p->type;

	pkt.length=hton(sizeof(pkt));

	pkt.tx_seconds=p->tx_seconds;
	pkt.tx_nanoseconds=p->tx_nanoseconds;
	pkt.rx_seconds=desc.seconds;
	pkt.rx_nanoseconds=desc.nanoseconds;
	pkt.integrator=err;
	pkt.timer=error/1ns;
	transmit(p_eth);
    }
}

void udp_sync::Transmitted(
    Ethernet *eth,
    Ethernet::descriptor const &desc
) {
}

void udp_sync::Unreachable(Ethernet *eth)
{
    eth->erase_udp_transmitter(this,pkt.dst_port);
    pkt.length=0;
}
