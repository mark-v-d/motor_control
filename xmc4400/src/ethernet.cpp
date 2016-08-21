#include "ethernet.h"

#define PHY_ADDR 0

#define XMC_ETH_MAC_NUM_RX_BUF (4)
#define XMC_ETH_MAC_NUM_TX_BUF (4)

/* MAC ADDRESS*/
#define MAC_ADDR0   0x00
#define MAC_ADDR1   0x00
#define MAC_ADDR2   0x45
#define MAC_ADDR3   0x19
#define MAC_ADDR4   0x03
#define MAC_ADDR5   0x00
#define MAC_ADDR    ((uint64_t)MAC_ADDR0 | \
                     ((uint64_t)MAC_ADDR1 << 8) | \
                     ((uint64_t)MAC_ADDR2 << 16) | \
                     ((uint64_t)MAC_ADDR3 << 24) | \
                     ((uint64_t)MAC_ADDR4 << 32) | \
                     ((uint64_t)MAC_ADDR5 << 40))

static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t 
    rx_desc[XMC_ETH_MAC_NUM_RX_BUF] __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t 
    tx_desc[XMC_ETH_MAC_NUM_TX_BUF] __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) uint8_t 
    rx_buf[XMC_ETH_MAC_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE] 
    __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) uint8_t 
    tx_buf[XMC_ETH_MAC_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE] 
    __attribute__((section ("ETH_RAM")));

static XMC_ETH_PHY_CONFIG_t eth_phy_config={
    .interface = XMC_ETH_LINK_INTERFACE_RMII,
    .speed = XMC_ETH_LINK_SPEED_100M,
    .duplex = XMC_ETH_LINK_DUPLEX_FULL,
    .enable_auto_negotiate = true,
    .enable_loop_back = false
};

XMC_ETH_MAC_t Ethernet::eth_mac={
    .regs = ETH0,
    .address = MAC_ADDR,
    .rx_desc = rx_desc,
    .tx_desc = tx_desc,
    .rx_buf = &rx_buf[0][0],
    .tx_buf = &tx_buf[0][0],
    .frame_end=NULL,
    .num_rx_buf = XMC_ETH_MAC_NUM_RX_BUF,
    .num_tx_buf = XMC_ETH_MAC_NUM_TX_BUF,
    .tx_index=0,
    .rx_index=0,
    .tx_ts_index=0
};

std::atomic<int> Ethernet::rx_pending(0);

void Ethernet::FinishInit(XMC_ETH_MAC_PORT_CTRL_t const &port_control)
{
    XMC_ETH_MAC_SetPortControl(&eth_mac, port_control);
    XMC_ETH_MAC_Init(&eth_mac);

    XMC_ETH_MAC_DisableJumboFrame(&eth_mac);


    XMC_ETH_PHY_Init(&eth_mac, PHY_ADDR, &eth_phy_config);

    while(XMC_ETH_PHY_GetLinkStatus(&eth_mac, PHY_ADDR)!=XMC_ETH_LINK_STATUS_UP)
	;

    XMC_ETH_LINK_SPEED_t speed=XMC_ETH_PHY_GetLinkSpeed(&eth_mac, PHY_ADDR);
    XMC_ETH_LINK_DUPLEX_t duplex=XMC_ETH_PHY_GetLinkDuplex(&eth_mac, PHY_ADDR);

    XMC_ETH_MAC_SetLink(&eth_mac, speed, duplex);

    /* Enable ethernet interrupts */
    XMC_ETH_MAC_EnableEvent(&eth_mac, XMC_ETH_MAC_EVENT_RECEIVE);

    NVIC_SetPriority(ETH0_0_IRQn, 
	NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_ClearPendingIRQ(ETH0_0_IRQn);
    NVIC_EnableIRQ(ETH0_0_IRQn);

    XMC_ETH_MAC_EnableTx(&eth_mac);
    XMC_ETH_MAC_EnableRx(&eth_mac);
}

extern "C" void ETH0_0_IRQHandler(void)
{
    XMC_ETH_MAC_ClearEventStatus(&Ethernet::eth_mac, XMC_ETH_MAC_EVENT_RECEIVE);
    Ethernet::rx_pending++;
}
