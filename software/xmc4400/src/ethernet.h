#ifndef ETHERNET_H
#define ETHERNET_H

#include <array>
#include <map>
#include <atomic>
#include "gpio.h"
#include "packet.h"
#include "atomic_wrapping_counter.h"

namespace ethernet {
#include "ethernet_map.h"
}

class Ethernet {
public:
    class Transmitter;

    struct descriptor {
	std::atomic<uint32_t> status;
	uint32_t length;
	packet *buffer;
	descriptor *next;
	uint32_t ext_status;
	uint32_t reserved;
	uint32_t nanoseconds;
	uint32_t seconds;
	Transmitter *txp;
    public: // types
	enum protocol_t {
	    UNKNOWN=0,
	    UDP=1,
	    TCP=2,
	    ICMP=3
	};
	enum rx_status_t {
	    OWN   =(0x80000000U), /**< Own bit 1=DMA, 0=CPU */
	    AFM   =(0x40000000U), /**< Destination address filter fail*/
	    FL    =(0x3FFF0000U), /**< Frame length mask */
	    ES    =(0x00008000U), /**< Error summary */
	    DE    =(0x00004000U), /**< Descriptor error */
	    SAF   =(0x00002000U), /**< Source address filter fail */
	    LE    =(0x00001000U), /**< Length error */
	    OE    =(0x00000800U), /**< Overflow error */
	    VLAN  =(0x00000400U), /**< VLAN tag */
	    RX_FS =(0x00000200U), /**< First descriptor */
	    RX_LS =(0x00000100U), /**< Last descriptor */
	    TSA   =(0x00000080U), /**< Timestamp available */
	    RX_LC =(0x00000040U), /**< Late collision */
	    FT    =(0x00000020U), /**< Frame type */
	    RWT   =(0x00000010U), /**< Receive watchdog timeout */
	    RE    =(0x00000008U), /**< Receive error */
	    DBE   =(0x00000004U), /**< Dribble bit error */
	    CE    =(0x00000002U), /**< CRC error */
	    ESA   =(0x00000001U)  /**< Extended Status/Rx MAC address */
	};
	enum tx_status_t {
	    //OWN =(0x80000000U), /**< Own bit 1=DMA, 0=CPU */
	    IC    =(0x40000000U), /**< Interrupt on competition */
	    TX_LS =(0x20000000U), /**< Last segment */
	    TX_FS =(0x10000000U), /**< First segment */
	    DC    =(0x08000000U), /**< Disable CRC */
	    DP    =(0x04000000U), /**< Disable pad */
	    TTSE  =(0x02000000U), /**< Transmit time stamp enable */
	    CIC   =(0x00C00000U), /**< Checksum insertion control */
	    TER   =(0x00200000U), /**< Transmit end of ring */
	    TCH   =(0x00100000U), /**< Second address chained */
	    TTSS  =(0x00020000U), /**< Transmit time stamp status */
	    IHE   =(0x00010000U), /**< IP header error */
	    //ES  =(0x00008000U), /**< Error summary */
	    JT    =(0x00004000U), /**< Jabber timeout */
	    FF    =(0x00002000U), /**< Frame flushed */
	    IPE   =(0x00001000U), /**< IP payload error */
	    LOC   =(0x00000800U), /**< Loss of carrier */
	    NC    =(0x00000400U), /**< No carrier */
	    TX_LC =(0x00000200U), /**< Late collision */
	    EC    =(0x00000100U), /**< Excessive collision */
	    VF    =(0x00000080U), /**< VLAN frame */
	    CC    =(0x00000078U), /**< Collision count */
	    ED    =(0x00000004U), /**< Excessive deferral */
	    UF    =(0x00000002U), /**< Underflow error */
	    DB    =(0x00000001U) /**< Deferred bit */
	};
	enum rx_length {
	    LENGTH_MASK = 0x1fff,
	    CHAINED = 0x4000
	};

	enum ext_status_t {
	    PTP_version 	=0x2000, // 0=v1, 1=v2 (when msg_type!=0)
	    PTP_udp		=0x1000, // PTP over 0=raw, 1=UDP
	    PTP_msg		=0x0f00, // PTP message_type
	    PTP_msg_noptp	=0,	 // not a PTP message
	    PTP_msg_SYNC	=0x0100,
	    PTP_msg_Follow_Up	=0x0200,
	    PTP_msg_Delay_Req	=0x0300,
	    PTP_msg_Delay_Resp	=0x0400,
	    PTP_msg_Pdelay_Req	=0x0500,
	    PTP_msg_Pdelay_Resp	=0x0600,
	    PTP_msg_Pdelay_Resp_Follow_Up=0x0700,
	    PTP_msg_Announce	=0x0800,
	    PTP_msg_Management	=0x0900,
	    PTP_msg_Signaling	=0x0a00,
	    IPv6		=0x0080,
	    IPv4		=0x0040,
	    IP_cksum_bypassed	=0x0020,
	    IP_payload_error	=0x0010,
	    IP_header_error	=0x0008,
	    IP_payload_type	=0x0007
	};
    public: // functions
	int packetType(void) { return ext_status&IP_payload_type; }
    };

    class Transmitter {
    public:
	virtual void Transmitted(Ethernet*,descriptor const&)=0;
	virtual void Unreachable(Ethernet*)=0;
    };

    class Receiver {
    public:
	virtual void Received(Ethernet*,descriptor const&)=0;
    };


    std::map<uint16_t,Receiver*> udp;
    std::map<uint16_t,Transmitter*> udp_tx;

private:
    std::array<uint8_t,4> src_ip;

    static Ethernet *instance;
    friend void ETH0_0_IRQHandler(uint32_t event);

    uint8_t phy_addr;

    enum { tx_buffer_count=4 };
    static std::array<descriptor,tx_buffer_count> txd;
    atomic_wrapping_counter<int,tx_buffer_count> tx_put;

    enum { rx_buffer_count=4 };
    static std::array<descriptor,rx_buffer_count> rxd;
    static std::array<packet,rx_buffer_count> rxp;
    int rx_get;

    Receiver *icmpHandler;
public:
    Ethernet(void):tx_put(0), rx_get(0) {}
    template <typename rxd0_t, typename rxd1_t, typename clk_rmii_t,
	typename crs_dv_t, typename rxer_t, typename txd0_t, typename txd1_t,
	typename txen_t,typename mdc_t, typename mdo_t
    >
    void init(uint8_t pa, rxd0_t rxd0, rxd1_t rxd1,
	clk_rmii_t clk_rmii, crs_dv_t crs_dv, rxer_t rxer,
	txd0_t txd0,
	txd1_t txd1, txen_t txen,
	mdc_t mdc, mdo_t mdo,

	Receiver *icmp
    );

    int transmit(Transmitter *tx,void *data,size_t size);
    void add_udp_receiver(Receiver *rx,int16_t port) { udp[port]=rx; }
    void add_udp_transmitter(Transmitter *tx,int16_t port) { udp_tx[port]=tx; }
    void erase_udp_transmitter(Transmitter *tx,int16_t port);

    void Unreachable_udp(uint16_t port) {
	if(udp_tx.count(port))
	    udp_tx[port]->Unreachable(this);
    }

    void set_saddr(ethernet_t *p);
    void set_saddr(ipv4_t *p);
    void set_ipv4_address(std::array<uint8_t,4> s) {
	if(s[3]!=255)
	    src_ip=s;
    }

    struct timestamp_t {
	uint32_t s;
	uint32_t ns;


	auto operator -(timestamp_t o) {
	    return std::chrono::duration<float>(int32_t(s-o.s)
		+1.0e-9f*int32_t(ns-o.ns));
	}
    };

    auto system_time() {
	return timestamp_t(eth.SYSTEM_TIME_SECONDS,eth.SYSTEM_TIME_NANOSECONDS);
    }
    auto target_time() {
	return timestamp_t(eth.TARGET_TIME_SECONDS,eth.TARGET_TIME_NANOSECONDS);
    }

private:
    void FinishInit();
    void SetManagmentClockDivider(void);
    void SetAddress();

    void PHY_Init(void);
    void WritePhy(uint8_t reg_addr, uint16_t data);
    uint16_t ReadPhy(uint8_t reg_addr);

    void receiveIRQ(void);
    void transmitIRQ(void);
    XMC_ETH_MAC_PORT_CTRL_t port_control;
};

template <typename rxd0_t, typename rxd1_t, typename clk_rmii_t,
    typename crs_dv_t, typename rxer_t, typename txd0_t, typename txd1_t,
    typename txen_t,typename mdc_t, typename mdo_t
>
void Ethernet::init(
    uint8_t pa,
    rxd0_t rxd0,
    rxd1_t rxd1,
    clk_rmii_t clk_rmii,
    crs_dv_t crs_dv,
    rxer_t rxer,

    txd0_t txd0,
    txd1_t txd1,
    txen_t txen,
    mdc_t mdc,
    mdo_t mdo,

    Receiver *icmp
)
{
    phy_addr=pa;
    icmpHandler=icmp;

    txd0.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    txd0.set(ethernet::TXD0(txd0));
    txd1.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    txd1.set(ethernet::TXD1(txd1));
    txen.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    txen.set(ethernet::TXEN(txen));
    mdc.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    mdc.set(ethernet::MDC(mdc));
    mdo.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    mdo.set(ethernet::MDIO_out(mdo));


    auto make_input=[](auto ...x) {
	(x.set(XMC_GPIO_MODE_INPUT_TRISTATE), ...);
	(x.input_enable(), ...);
    };
    make_input(rxd0, rxd1, clk_rmii, crs_dv, rxer);

    ETH0_CON->CON=
	bitfield<ETH_CON_RXD0_Msk>(ethernet::RXD0(rxd0)) |
	bitfield<ETH_CON_RXD1_Msk>(ethernet::RXD1(rxd1)) |
	bitfield<ETH_CON_CLK_RMII_Msk>(ethernet::CLK_RMII(clk_rmii)) |
	bitfield<ETH_CON_CRS_DV_Msk>(ethernet::CRS_DV(crs_dv)) |
	bitfield<ETH_CON_RXER_Msk>(ethernet::RXER(rxer)) |
	bitfield<ETH_CON_MDIO_Msk>(ethernet::MDIO_in(mdo)) |
	ETH_CON_INFSEL_Msk;
    FinishInit();
}


inline void Ethernet::set_saddr(ethernet_t *p)
{
    for(int i=0;i<p->src_mac.size(); i++)
	p->src_mac[i]=g_chipid[i];

    p->src_mac[0]&=~1;
    p->src_mac[0]|=2;
}

inline void Ethernet::set_saddr(ipv4_t *p)
{
    set_saddr((ethernet_t*)p);
    p->src_ip=src_ip;
}

#endif
