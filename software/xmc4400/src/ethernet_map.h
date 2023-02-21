////////////////////////////////////////////////////////////////////////////////
// ETH0_CON=0x4404e0f klopt
// RXD0->D 5.0
// RXD1->D 5.1
// RXD2->A
// RXD3->A
// CLK_RMII->C 15.8
// CRS_DV->D 5.2
// CRS->A
// RXER->B 0.11
// COL->A
// CLK_TX->A
// MDIO->B 2.0
// INFSEL=1

template <int PORT, int PIN> constexpr int CLK_RMII(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an CLK_RMII pin"); return 0; }
template <int PORT, int PIN> constexpr int COL(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an COL pin"); return 0; }
template <int PORT, int PIN> constexpr int CRS_DV(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an CRS_DV pin"); return 0; }
template <int PORT, int PIN> constexpr int RXD0(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an RXD0 pin"); return 0; }
template <int PORT, int PIN> constexpr int RXD1(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an RXD1 pin"); return 0; }
template <int PORT, int PIN> constexpr int RXER(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an RXER pin"); return 0; }

template<> constexpr int CLK_RMII(gpio::pin<0,0>)	{ return 1; }
template<> constexpr int CLK_RMII(gpio::pin<15,8>)	{ return 2; }
template<> constexpr int CLK_RMII(gpio::pin<2,1>)	{ return 0; }
template<> constexpr int COL(gpio::pin<2,15>)		{ return 0; }
template<> constexpr int CRS_DV(gpio::pin<2,5>)		{ return 0; }
template<> constexpr int CRS_DV(gpio::pin<0,1>)		{ return 1; }
template<> constexpr int CRS_DV(gpio::pin<15,9>)	{ return 2; }
template<> constexpr int CRS_DV(gpio::pin<5,2>)		{ return 3; }
template<> constexpr int RXD0(gpio::pin<0,2>)		{ return 1; }
template<> constexpr int RXD0(gpio::pin<14,8>)		{ return 2; }
template<> constexpr int RXD0(gpio::pin<2,2>)		{ return 0; }
template<> constexpr int RXD0(gpio::pin<5,0>)		{ return 3; }
template<> constexpr int RXD1(gpio::pin<0,3>)		{ return 1; }
template<> constexpr int RXD1(gpio::pin<14,9>)		{ return 2; }
template<> constexpr int RXD1(gpio::pin<2,3>)		{ return 0; }
template<> constexpr int RXD1(gpio::pin<5,1>)		{ return 3; }
template<> constexpr int RXER(gpio::pin<0,11>)		{ return 1; }
template<> constexpr int RXER(gpio::pin<2,4>)		{ return 0; }

template <int PORT, int PIN> constexpr XMC_GPIO_MODE_t MDC(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an MDC pin"); return XMC_GPIO_MODE_OUTPUT_ALT1; }
template <int PORT, int PIN> constexpr XMC_GPIO_MODE_t TXD0(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an TXD0 pin"); return XMC_GPIO_MODE_OUTPUT_ALT1; }
template <int PORT, int PIN> constexpr XMC_GPIO_MODE_t TXD1(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an TXD1 pin"); return XMC_GPIO_MODE_OUTPUT_ALT1; }
template <int PORT, int PIN> constexpr XMC_GPIO_MODE_t TXEN(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an TXEN pin"); return XMC_GPIO_MODE_OUTPUT_ALT1; }

template<> constexpr XMC_GPIO_MODE_t MDC(gpio::pin<0,10>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t MDC(gpio::pin<1,10>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t MDC(gpio::pin<2,7>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD0(gpio::pin<0,5>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD0(gpio::pin<1,13>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD0(gpio::pin<2,8>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD1(gpio::pin<0,6>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD1(gpio::pin<1,14>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXD1(gpio::pin<2,9>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXEN(gpio::pin<0,4>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXEN(gpio::pin<1,12>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }
template<> constexpr XMC_GPIO_MODE_t TXEN(gpio::pin<2,5>)	{ return XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1; }

template <int PORT, int PIN> constexpr int MDIO_in(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an MDIO pin"); return 0; }

template<> constexpr int MDIO_in(gpio::pin<0,9>)	 { return 0; } // HWO0
template<> constexpr int MDIO_in(gpio::pin<1,11>)	 { return 2; }
template<> constexpr int MDIO_in(gpio::pin<2,0>)	 { return 1; }

template <int PORT, int PIN> constexpr XMC_GPIO_HWCTRL_t MDIO_out(gpio::pin<PORT,PIN>) { static_assert(PORT==-1, "Not an MDIO pin"); return XMC_GPIO_HWCTRL_PERIPHERAL1; }

template<> constexpr XMC_GPIO_HWCTRL_t MDIO_out(gpio::pin<0,9>)	 { return XMC_GPIO_HWCTRL_PERIPHERAL1; } // HWO0
template<> constexpr XMC_GPIO_HWCTRL_t MDIO_out(gpio::pin<1,11>) { return XMC_GPIO_HWCTRL_PERIPHERAL1; }
template<> constexpr XMC_GPIO_HWCTRL_t MDIO_out(gpio::pin<2,0>)	 { return XMC_GPIO_HWCTRL_PERIPHERAL1; }
