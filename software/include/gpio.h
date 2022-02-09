#ifndef GPIO_H
#define GPIO_H

#include <xmc_device.h>
#include <xmc_gpio.h>
#include <xmc_eth_mac.h>
#include <xmc_ccu8.h>
#include <xmc_uart.h>
#include <xmc_dma.h>
#include <xmc_scu.h>

#include "misc.h"

inline XMC_GPIO_MODE_t operator | (XMC_GPIO_MODE_t a, XMC_GPIO_MODE_t b)
{
    return XMC_GPIO_MODE_t(int(a)|int(b));
}

namespace gpio {

struct XMC_GPIO_PORT_padded:public XMC_GPIO_PORT
{
  __I  uint32_t  RESERVED5[34];
};

extern XMC_GPIO_PORT_padded port[15];

template <int PORT, int PIN>
class pin
{
public:
    static constexpr int PORT_c=PORT;
    static constexpr int PIN_c=PIN;
    constexpr pin() { static_assert(PORT>=0 && PORT<=15,
	"Illegal port, should be 0..15");
    }

    operator int(void) {
	return XMC_GPIO_GetInput(&port[PORT], PIN);
    }

    operator bool(void) {
	return XMC_GPIO_GetInput(&port[PORT], PIN);
    }

    void set(XMC_GPIO_MODE_t m) {
	// Isn't this byte adressable?
	// XMC_GPIO_SetMode is not inline
	auto x=port[PORT].IOCR[PIN/4];
	x&=~(255<<8*(PIN%4));
	x|=m<<8*(PIN%4);
	port[PORT].IOCR[PIN/4]=x;
    }

    void set(XMC_GPIO_HWCTRL_t  m) {
	// XMC_GPIO_SetHardwareControl is not inline
	auto x=port[PORT].HWSEL;
	x&=~(3<<2*PIN);
	x|=(m&3)<<2*PIN;
	port[PORT].HWSEL=x;
    }

    void input_enable(void) {
	XMC_GPIO_EnableDigitalInput(&port[PORT],PIN);
    }

    void input_disable(void) {
	XMC_GPIO_DisableDigitalInput(&port[PORT],PIN);
    }

    void powersave(int i) {
	if(i)
	    XMC_GPIO_EnablePowerSaveMode(&port[PORT], PIN);
	else
	    XMC_GPIO_DisablePowerSaveMode(&port[PORT], PIN);
    }

#if UC_FAMILY == XMC4
    void set(XMC_GPIO_OUTPUT_STRENGTH_t m) {
	auto x=port[PORT].PDR[PIN/8];
	x&=~(15<<4*(PIN%8));
	x|=m<<4*(PIN%8);
	port[PORT].PDR[PIN/8]=x;
    }

    /* REMOVE THIS !!!! */
    // Ethernet inputs
    operator XMC_ETH_MAC_PORT_CTRL_RXD0() {
	static_assert(PORT==-1, "Cannot use this pin as RXD0 for ETH0");
	return -1;
    }
    operator XMC_ETH_MAC_PORT_CTRL_RXD1() {
	static_assert(PORT==-1, "Cannot use this pin as RXD1 for ETH0");
	return -1;
    }
    operator XMC_ETH_MAC_PORT_CTRL_CLK_RMII() {
	static_assert(PORT==-1, "Cannot use this pin as CLK_RMII for ETH0");
	return -1;
    }
    operator XMC_ETH_MAC_PORT_CTRL_CRS_DV() {
	static_assert(PORT==-1, "Cannot use this pin as CRS_DV for ETH0");
	return -1;
    }
    operator XMC_ETH_MAC_PORT_CTRL_RXER() {
	static_assert(PORT==-1, "Cannot use this pin as RXER for ETH0");
	return -1;
    }
    operator XMC_ETH_MAC_PORT_CTRL_MDIO() {
	static_assert(PORT==-1, "Cannot use this pin as MDIO for ETH0");
	return -1;
    }
#endif
};

template <int PORT, int PIN>
class output:public pin<PORT,PIN>
{
public:
    int operator=(int i) { port[PORT].OMR=(i? 1:0x10000)<<PIN; return i; }
    void operator^=(int i) { if(i) port[PORT].OMR=0x10001<<PIN; }
    void toggle(void) { port[PORT].OMR=0x10001<<PIN; }
};

}


#if UC_FAMILY == XMC4
// ETH0 Ports //////////////////////////////////////////////////////////////////
// REMOVE THIS
namespace gpio {

#define type_conversion(type,PIN,PORT) \
template <> \
inline gpio::pin<PIN,PORT>::operator type() \
{ \
    set(XMC_GPIO_MODE_INPUT_TRISTATE); \
    return type##_P##PIN##_##PORT; \
}


type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD0,2,2);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD0,0,2);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD0,14,8);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD0,5,0);

type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD1,2,3);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD1,0,3);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD1,14,9);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXD1,5,1);

type_conversion(XMC_ETH_MAC_PORT_CTRL_CLK_RMII,2,1);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CLK_RMII,0,0);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CLK_RMII,15,8);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CLK_RMII,6,5);

type_conversion(XMC_ETH_MAC_PORT_CTRL_CRS_DV,2,5);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CRS_DV,0,1);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CRS_DV,15,9);
type_conversion(XMC_ETH_MAC_PORT_CTRL_CRS_DV,5,2);

type_conversion(XMC_ETH_MAC_PORT_CTRL_RXER,2,4);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXER,0,11);
type_conversion(XMC_ETH_MAC_PORT_CTRL_RXER,5,3);

type_conversion(XMC_ETH_MAC_PORT_CTRL_MDIO,0,9);
type_conversion(XMC_ETH_MAC_PORT_CTRL_MDIO,2,0);
type_conversion(XMC_ETH_MAC_PORT_CTRL_MDIO,1,11);

#undef type_conversion
#include "gpio_output_conversions"
}

#include "input_conversions.h"
#endif

#endif
