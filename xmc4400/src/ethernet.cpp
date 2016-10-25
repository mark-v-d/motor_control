#include "ethernet.h"
#include "xmc_scu.h"

Ethernet *Ethernet::instance=0;

enum { tx_buffer_count=4 };
std::array<Ethernet::descriptor,tx_buffer_count> Ethernet::txd __attribute__((section ("ETH_RAM")));

enum { rx_buffer_count=4 };
std::array<Ethernet::descriptor,rx_buffer_count> Ethernet::rxd __attribute__((section ("ETH_RAM")));
std::array<packet,rx_buffer_count> Ethernet::rxp __attribute__((section ("ETH_RAM")));


enum PHY_stuff {
    PHY_ID1=0x0022,
    PHY_ID2_KSZ8081RNA=0x1560,

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

void Ethernet::SetManagmentClockDivider(void)
{
    enum  div_t {
	XMC_ETH_MAC_MDC_DIVIDER_16  =(2U << ETH_GMII_ADDRESS_CR_Pos) ,
	XMC_ETH_MAC_MDC_DIVIDER_26  =(3U << ETH_GMII_ADDRESS_CR_Pos),
	XMC_ETH_MAC_MDC_DIVIDER_42  =(0U << ETH_GMII_ADDRESS_CR_Pos),
	XMC_ETH_MAC_MDC_DIVIDER_62  =(1U << ETH_GMII_ADDRESS_CR_Pos),
	XMC_ETH_MAC_MDC_DIVIDER_102 =(4U << ETH_GMII_ADDRESS_CR_Pos),
	XMC_ETH_MAC_MDC_DIVIDER_124 =(5U << ETH_GMII_ADDRESS_CR_Pos)
    };

    uint32_t eth_mac_clk=XMC_SCU_CLOCK_GetSystemClockFrequency() >> 1U;
    if (eth_mac_clk <= 35000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_16;
    } else if (eth_mac_clk <= 60000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_26;
    } else if (eth_mac_clk <= 100000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_42;
    } else if (eth_mac_clk <= 150000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_62;
    } else if (eth_mac_clk <= 200000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_102;
    } else if (eth_mac_clk <= 250000000UL) {
	ETH0->GMII_ADDRESS = XMC_ETH_MAC_MDC_DIVIDER_124;
    }
}

void Ethernet::SetAddress(uint64_t addr)
{
  ETH0->MAC_ADDRESS0_HIGH = (uint32_t)(addr >> 32);
  ETH0->MAC_ADDRESS0_LOW = (uint32_t)addr;
}

uint16_t Ethernet::ReadPhy(uint8_t reg_addr)
{
    ETH0->GMII_ADDRESS=
	(ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_CR_Msk) |
	ETH_GMII_ADDRESS_MB_Msk |
	((uint32_t)phy_addr << ETH_GMII_ADDRESS_PA_Pos) |
	((uint32_t)reg_addr << ETH_GMII_ADDRESS_MR_Pos);

    /* Poll busy bit during max PHY_TIMEOUT time */
    for(int retries=0; retries<XMC_ETH_MAC_PHY_MAX_RETRIES; retries++) {
	if((ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_MB_Msk) == 0U) {
	    return ETH0->GMII_DATA & ETH_GMII_DATA_MD_Msk;
	}
    };
}

void Ethernet::WritePhy(uint8_t reg_addr, uint16_t data)
{
    ETH0->GMII_DATA=data;
    ETH0->GMII_ADDRESS = 
	(ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_CR_Msk) |
	ETH_GMII_ADDRESS_MB_Msk | ETH_GMII_ADDRESS_MW_Msk |
	((uint32_t)phy_addr << ETH_GMII_ADDRESS_PA_Pos) |
	((uint32_t)reg_addr << ETH_GMII_ADDRESS_MR_Pos);

    for(int retries=0; retries<XMC_ETH_MAC_PHY_MAX_RETRIES; retries++) {
	if((ETH0->GMII_ADDRESS & ETH_GMII_ADDRESS_MB_Msk) == 0U)
	    return;
    }
}

void Ethernet::PHY_Init(void)
{
    uint16_t id1=ReadPhy(REG_PHYIDR1);
    uint16_t id2=ReadPhy(REG_PHYIDR2);
    if(id1==PHY_ID1 && (id2&0xfff0)==PHY_ID2_KSZ8081RNA) {
	WritePhy(REG_BMCR, BMCR_RESET);
	WritePhy(REG_BMCR, BMCR_DUPLEX | BMCR_ANEG_EN);

	uint16_t status;
	do {
	    status=ReadPhy(REG_BMSR);
	} while(!(status&BMSR_LINK_STAT));
    } // else hope for the best
}

void Ethernet::FinishInit(XMC_ETH_MAC_PORT_CTRL_t const &port_control)
{
    ETH0_CON->CON = (uint32_t)port_control.raw;
    instance=this;

    XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_ETH0);
    XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_ETH);
#if UC_DEVICE != XMC4500
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_ETH0);
#endif
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_ETH0);

    ETH0->BUS_MODE |= (uint32_t)ETH_BUS_MODE_SWR_Msk;
    while ((ETH0->BUS_MODE & (uint32_t)ETH_BUS_MODE_SWR_Msk) != 0U)
	;

    SetManagmentClockDivider();

    SetAddress(mac_addr);

    ETH0->MAC_CONFIGURATION = (uint32_t)ETH_MAC_CONFIGURATION_IPC_Msk;

    ETH0->FLOW_CONTROL = ETH_FLOW_CONTROL_DZPQ_Msk; // Disable Zero Quanta Pause

    ETH0->OPERATION_MODE = (uint32_t)ETH_OPERATION_MODE_RSF_Msk |
			  (uint32_t)ETH_OPERATION_MODE_TSF_Msk;

    /* Increase enhanced descriptor to 8 WORDS, required when the Advanced
       Time-Stamp feature or Full IPC Offload Engine is enabled 
    */
    ETH0->BUS_MODE |= (uint32_t)ETH_BUS_MODE_ATDS_Msk;

    // Receive buffers and descriptors
    for(int i=0; i<rxp.size(); i++) {
	rxd[i].status=descriptor::OWN;
	rxd[i].length=descriptor::CHAINED | sizeof(packet);
	rxd[i].buffer=&rxp[i];
	rxd[i].next=&rxd[(i+1)%rxd.size()];
    }
    ETH0->RECEIVE_DESCRIPTOR_LIST_ADDRESS=uint32_t(&rxd[0]);

    // Transmit descriptors
    for(int i=0; i<txd.size(); i++) {
	txd[i].status=0;
	txd[i].length=0;
	txd[i].buffer=NULL;
	txd[i].next=&txd[(i+1)%txd.size()];
    }
    ETH0->TRANSMIT_DESCRIPTOR_LIST_ADDRESS=uint32_t(&txd[0]);

    //XMC_ETH_MAC_InitTxDescriptors(&eth_mac);

    PHY_Init();

    // Only full duplex/100Mb, no jumbo frames
    ETH0->MAC_CONFIGURATION &= (uint32_t)~ETH_MAC_CONFIGURATION_JE_Msk;
    ETH0->MAC_CONFIGURATION|= 
	ETH_MAC_CONFIGURATION_DM_Msk | ETH_MAC_CONFIGURATION_FES_Msk;


    ETH0->STATUS = 0xFFFFFFFFUL;
    ETH0->INTERRUPT_ENABLE=
	XMC_ETH_MAC_EVENT_RECEIVE|ETH_INTERRUPT_ENABLE_NIE_Msk;

    NVIC_SetPriority(ETH0_0_IRQn, 
	NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_ClearPendingIRQ(ETH0_0_IRQn);
    NVIC_EnableIRQ(ETH0_0_IRQn);

    ETH0->OPERATION_MODE|=
	ETH_OPERATION_MODE_SR_Msk | ETH_OPERATION_MODE_ST_Msk;
    ETH0->MAC_CONFIGURATION|=
	ETH_MAC_CONFIGURATION_RE_Msk | ETH_MAC_CONFIGURATION_TE_Msk;
}

void Ethernet::receiveIRQ(void)
{
    // Handle all packets not owned by the ethernet DMA
    while(!(rxd[rx_get].status & descriptor::OWN)) {
	switch(rxd[rx_get].packetType()) {
	case descriptor::UDP: {
	    uint16_t port=rxd[rx_get].buffer->udp.dst_port;
	    if(udp.count(port))
		udp[port]->Received(this,rxd[rx_get]);
	    } break;
	case descriptor::ICMP:                               
	    if(icmpHandler)                      
		icmpHandler->Received(this,rxd[rx_get]);
	    break;                           
	}
	rxd[rx_get].status=descriptor::OWN;
	if(++rx_get>=rxp.size())
	    rx_get=0;
    }
}


int Ethernet::transmit(
    Ethernet::Transmitter *tx,
    void *data,
    size_t size
) {
    int bufnum=tx_put++;
    txd[bufnum].buffer=static_cast<packet*>(data);
    txd[bufnum].length=size;
    txd[bufnum].txp=tx;
    txd[bufnum].status=descriptor::OWN | descriptor::IC | descriptor::TX_LS
	| descriptor::TX_FS | descriptor::TTSE | descriptor::CIC 
	| descriptor::TCH;
    ETH0->TRANSMIT_POLL_DEMAND=0;
}

void Ethernet::transmitIRQ(void)
{
}

void Ethernet::erase_udp_transmitter(Transmitter *tx,int16_t port)
{
    if(udp_tx.count(port) && udp_tx[port]==tx)
	udp_tx.erase(port);
}

inline void ETH0_0_IRQHandler(uint32_t event)
{
    if(event&XMC_ETH_MAC_EVENT_RECEIVE) {
	Ethernet::instance->receiveIRQ();
    }
    if(event&XMC_ETH_MAC_EVENT_TRANSMIT) {
	Ethernet::instance->transmitIRQ();
    }
}

extern "C" void ETH0_0_IRQHandler(void)
{
    ETH0_0_IRQHandler(ETH0->STATUS);
    ETH0->STATUS = 0xFFFFFFFFUL;
}
