#include "XMC4500.h"
#include "xmc_eth_mac_map.h"
#include "xmc_scu.h"
#include "gpio.h"
#include "packet.h"

#include <atomic>
// #include <system_error>
#include <array>
// #include <unordered_map>

namespace Ethernet {

typedef uint8_t MacAddress[6];

enum PHY_stuff {
    PHY_ID1=0x0022,
    PHY_ID2=0x1550,

    REG_BMCR=0x00,
    REG_BMSR=0x01,
    REG_PHYIDR1=0x02,
    REG_PHYIDR2=0x03,
    REG_ANAR=0x04,
    REG_ANLPAR=0x05,
    REG_ANER=0x06,
    REG_ANNPTR=0x07,

    REG_PHYCTRL1=0x1e,

    BMCR_RESET=0x8000,
    BMCR_LOOPBACK=0x4000,
    BMCR_SPEED_SEL=0x2000,
    BMCR_ANEG_EN=0x1000,
    BMCR_POWER_DOWN=0x0800,
    BMCR_ISOLATE=0x0400,
    BMCR_REST_ANEG=0x0200,
    BMCR_DUPLEX=0x0100,
    BMCR_COL_TEST=0x0080,

    BMSR_100B_T4=0x8000,
    BMSR_100B_TX_FD=0x4000,
    BMSR_100B_TX_HD=0x2000,
    BMSR_10B_T_FD=0x1000,
    BMSR_10B_T_HD=0x0800,
    BMSR_MF_PRE_SUP=0x0040,
    BMSR_ANEG_COMPL=0x0020,
    BMSR_REM_FAULT=0x0010,
    BMSR_ANEG_ABIL=0x0008,
    BMSR_LINK_STAT=0x0004,
    BMSR_JABBER_DET=0x0002,
    BMSR_EXT_CAPAB=0x0001,

    PHYCTRL1_OPMODE_SPEED=0x0003,
    PHYCTRL1_OPMODE_DUPLEX=0x0004
};

enum protocol_t {
    RAW,
    ICMP,
    UDP,
    TCP
};

enum DESCRIPTOR_STATUS {
    DESC_OWN
};

class EthernetTransmitter;

struct descriptor {
    uint32_t status;
    uint32_t length;
    packet *buffer;
    descriptor *next;
    uint32_t ext_status;
    uint32_t reserved;
    uint32_t seconds;     
    uint32_t nanoseconds;
    EthernetTransmitter *txp;
};

class EthernetTransmitter {
public:
    virtual void Transmitted(descriptor*)=0;
};

class EthernetReceiver {
public:
    virtual void Received(descriptor*)=0;
};

class Ethernet {
    std::array<descriptor,4> rxd, txd;
    std::array<packet,4> rxp, txp;
    int rx_put, rx_get, tx_put, tx_get;

    std::atomic<uint32_t> rx_pending;
    std::atomic<uint32_t> tx_pending;

    EthernetReceiver *icmp;
    // std::unordered_map<int,EthernetReceiver*> udp;

    uint8_t phy_addr;
public:
    template <int a,int b,int c,int d,int e,int f,int g,int h,int i, int j>
    Ethernet(
	MacAddress const &mac_a,
	uint16_t phy_a,
	XMC_ETH_MAC_PORT_CTRL_RXD0 RXD0, 
	XMC_ETH_MAC_PORT_CTRL_RXD1 RXD1,
	XMC_ETH_MAC_PORT_CTRL_CLK_RMII CLK_RMII,
	XMC_ETH_MAC_PORT_CTRL_CRS_DV CRS_DV,
	XMC_ETH_MAC_PORT_CTRL_RXER RXER,

	iopin::ETH0_TXD0<a,b> TXD0,
	iopin::ETH0_TXD1<c,d> TXD1,
	iopin::ETH0_TX_EN<e,f> TX_EN,
	iopin::ETH0_MDC<g,h> MDC,
	iopin::ETH0_MDO<i,j> MDO
    );

    void receive(protocol_t protocol, uint16_t port, EthernetReceiver *rcv);
    void send(packet * const data,int length, EthernetTransmitter *txf=0);

    void receiveIRQ(void);
    void transmitIRQ(void);

private:
    void FinishInit(MacAddress const &address, 
	XMC_ETH_MAC_PORT_CTRL_t const &port_control);
    void MAC_SetManagmentClockDivider(void);
    void MAC_Init(MacAddress const &addr);
    void PHY_Init(void);
    void Descriptor_Init(void);

    void WritePhy(uint8_t reg_addr, uint16_t data);
    uint16_t ReadPhy(uint8_t reg_addr);

};

template <int a,int b,int c,int d,int e,int f,int g,int h,int i, int j>
Ethernet::Ethernet(
    MacAddress const &mac_a,
    uint16_t phy_a,

    XMC_ETH_MAC_PORT_CTRL_RXD0 RXD0, 
    XMC_ETH_MAC_PORT_CTRL_RXD1 RXD1,
    XMC_ETH_MAC_PORT_CTRL_CLK_RMII CLK_RMII,
    XMC_ETH_MAC_PORT_CTRL_CRS_DV CRS_DV,
    XMC_ETH_MAC_PORT_CTRL_RXER RXER,

    iopin::ETH0_TXD0<a,b> TXD0,
    iopin::ETH0_TXD1<c,d> TXD1,
    iopin::ETH0_TX_EN<e,f> TX_EN,
    iopin::ETH0_MDC<g,h> MDC,
    iopin::ETH0_MDO<i,j> MDO	// HWCTRL
):phy_addr(phy_a), icmp(0)
{
    TXD0.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    TXD1.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    TX_EN.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    MDC.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    MDO.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);

    XMC_ETH_MAC_PORT_CTRL_t port_control;
    port_control.mode = XMC_ETH_MAC_PORT_CTRL_MODE_RMII;
    port_control.rxd0 = RXD0;
    port_control.rxd1 = RXD1;
    port_control.clk_rmii = CLK_RMII;
    port_control.crs_dv = CRS_DV;
    port_control.rxer = RXER;
    port_control.mdio = XMC_ETH_MAC_PORT_CTRL_MDIO(MDO);

    FinishInit(mac_a, port_control);
}

}
