#include <atomic>
#include "xmc_eth_mac.h"
#include "xmc_eth_phy.h"
#include "gpio.h"


class Ethernet {
    void FinishInit(XMC_ETH_MAC_PORT_CTRL_t const &port_control);
    int received;
public:
    static XMC_ETH_MAC_t eth_mac;
    static std::atomic<int> rx_pending;

    template <int a,int b,int c,int d,int e,int f,int g,int h,int i, int j>
    Ethernet(
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

    XMC_ETH_MAC_STATUS_t Transmit(uint8_t *buffer, int len) {
      return XMC_ETH_MAC_SendFrame(&eth_mac, buffer, len, 0);
    }
    int Receive(uint8_t *buffer, int buflen);
};


template <int a,int b,int c,int d,int e,int f,int g,int h,int i, int j>
Ethernet::Ethernet(
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
) {
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

    FinishInit(port_control);
}


inline int Ethernet::Receive(uint8_t *buffer,int buflen)
{
    if(!rx_pending)
	return 0;
    uint32_t len=XMC_ETH_MAC_GetRxFrameSize(&eth_mac);

    rx_pending--;

    if ((len>0) && (len<XMC_ETH_MAC_BUF_SIZE) && len<=buflen) {
	XMC_ETH_MAC_ReadFrame(&eth_mac, buffer, len);
	return len;
    } else {
        XMC_ETH_MAC_ReadFrame(&eth_mac, NULL, 0);
        return -1;
    }
}
