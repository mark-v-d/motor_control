#include "ethernet.h"


namespace Ethernet {

void Ethernet::MAC_SetManagmentClockDivider(void)
{
    uint32_t eth_mac_clk=XMC_SCU_CLOCK_GetEthernetClockFrequency();

    if (eth_mac_clk <= 35000000U)
	ETH0->GMII_ADDRESS=2;
    else if (eth_mac_clk <= 60000000U)
	ETH0->GMII_ADDRESS=3;
    else if (eth_mac_clk <= 100000000U)
	ETH0->GMII_ADDRESS=0;
    else if (eth_mac_clk <= 150000000U)
	ETH0->GMII_ADDRESS=1;
    else if (eth_mac_clk <= 250000000U)
	ETH0->GMII_ADDRESS=4;
    else if (eth_mac_clk <= 300000000U)
	ETH0->GMII_ADDRESS=4;
    else
	throw 1;
}

void Ethernet::Descriptor_Init(void)
{
    for(int i=0; i<rxd.size(); i++) {
	rxd[i].status=DESC_OWN;
	rxd[i].length=sizeof(packet);
	rxd[i].buffer=&rxp[i];
	rxd[i].next=&rxd[(i+1)%rxd.size()];
	rxd[i].txp=0;
    }
    rx_put=rx_get=0;
    ETH0->RECEIVE_DESCRIPTOR_LIST_ADDRESS=reinterpret_cast<uint32_t>(&rxd[0]);
    for(int i=0; i<txd.size(); i++) {
	txd[i].status=0;
	txd[i].length=sizeof(packet);
	txd[i].buffer=&txp[i];
	txd[i].next=&txd[(i+1)%txd.size()];
	txd[i].txp=0;
    }
    tx_put=tx_get=0;
    ETH0->TRANSMIT_DESCRIPTOR_LIST_ADDRESS=reinterpret_cast<uint32_t>(&txd[0]);
}

void Ethernet::MAC_Init(MacAddress const &addr)
{
    SCU_CLK->CLKSET=XMC_SCU_CLOCK_ETH;
#if UC_DEVICE != XMC4500
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_ETH0);
#endif
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_ETH0);

    ETH0->BUS_MODE |= 
	ETH_BUS_MODE_SWR_Msk;	// reset
    while(ETH0->BUS_MODE & ETH_BUS_MODE_SWR_Msk)
	;

    MAC_SetManagmentClockDivider();

    ETH0->MAC_ADDRESS0_HIGH=((uint32_t)addr[5]<<8) | addr[4];
    ETH0->MAC_ADDRESS0_LOW=
	((uint32_t)addr[3]<<24) |
	((uint32_t)addr[2]<<16) |
	((uint32_t)addr[1]<<8) |
	((uint32_t)addr[0]<<0);

    /* Initialize MAC configuration */
    ETH0->MAC_CONFIGURATION=ETH_MAC_CONFIGURATION_IPC_Msk;

    /* Initialize Filter registers */
    ETH0->FLOW_CONTROL=ETH_FLOW_CONTROL_DZPQ_Msk; 

    ETH0->OPERATION_MODE= 
	ETH_OPERATION_MODE_RSF_Msk | 
	ETH_OPERATION_MODE_TSF_Msk;

    ETH0->BUS_MODE|=ETH_BUS_MODE_ATDS_Msk;	// 8 word descriptors

    /* Clear interrupts */
    ETH0->STATUS = 0xFFFFFFFFUL;
}

void Ethernet::PHY_Init(void)
{
    if(	ReadPhy(REG_PHYIDR1)!=PHY_ID1
	// || mac->ReadPhy(REG_PHYIDR2)&0xfff0!=PHY_ID2
    )
	throw 1;
    WritePhy(REG_BMCR, BMCR_RESET);
    WritePhy(REG_BMCR, BMCR_DUPLEX | BMCR_ANEG_EN);

    while(0 || !(ReadPhy(REG_BMSR) & BMSR_LINK_STAT))
	;
}

void Ethernet::FinishInit(
    MacAddress const &mac_a,
    XMC_ETH_MAC_PORT_CTRL_t const &port_control
) {
    ETH0_CON->CON = port_control.raw;

    Descriptor_Init();
    MAC_Init(mac_a);
    PHY_Init();

    // TODO: Verify PHY settings (full duplex/100Mb)

    ETH0->MAC_CONFIGURATION|=
	ETH_MAC_CONFIGURATION_DM_Msk  |	// full duplex
	ETH_MAC_CONFIGURATION_FES_Msk;	// 100Mb
    ETH0->INTERRUPT_ENABLE= 
	ETH_INTERRUPT_ENABLE_RIE_Msk |	// Receive interrupt
	ETH_INTERRUPT_ENABLE_TIE_Msk |	// Transmit interrupt
	ETH_INTERRUPT_ENABLE_NIE_Msk;	// Normal interrupts

    NVIC_SetPriority(ETH0_0_IRQn, 
	NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_ClearPendingIRQ(ETH0_0_IRQn);
    NVIC_EnableIRQ(ETH0_0_IRQn);

    ETH0->OPERATION_MODE|=
	ETH_OPERATION_MODE_ST_Msk |
	ETH_OPERATION_MODE_SR_Msk;
    ETH0->MAC_CONFIGURATION|=
	ETH_MAC_CONFIGURATION_TE_Msk |
	ETH_MAC_CONFIGURATION_RE_Msk;
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
    // throw(std::system_error);
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
    // throw(std::system_error);
}

#if 0
void Ethernet::receiveIRQ(void)
{
    switch(rxd[rx_get].packetType()) {
    case UDP:
	auto handler=udp.find(rx_desc->packet.udp.dst_port);
	if(handler!=udp.end())
	    handler->ReceivePacket(rx_desc);
	rx_desc.push_back();
	break;
    case ICMP:
	if(icmpHandler)
	    icmpHandler->ReceivePacker(rx_desc);
	break;
    }
    rx_get++
}
#else
void Ethernet::receiveIRQ(void)
{
    rx_pending++;
}
void Ethernet::transmitIRQ(void)
{
    tx_pending--;

}

void Ethernet::receive(
    protocol_t protocol, 
    uint16_t port, 
    EthernetReceiver *rcv
) {
    switch(protocol) {
    case ICMP: icmp=rcv; break;
    case UDP: udp[port]=rcv; break;
    }
}

void Ethernet::send(
    packet * const data,
    int length, 
    EthernetTransmitter *txf
) { 
    txd[tx_put].length=length;
    txd[tx_put].buffer=data;
    txd[tx_put].txp=txf;
    txd[tx_put++].status=DESC_OWN;
    tx_pending++;
}

#endif


}
