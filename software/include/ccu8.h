#ifndef CCU8_H
#define CCU8_H

#include "gpio.h"
#include <chrono>

namespace ccu8 {

typedef std::chrono::duration<float,std::ratio<1,FREQUENCY>> resolution_t;

struct ccu8_t:public CCU8_GLOBAL_TypeDef {
    const uint32_t RESERVED[(256-sizeof(CCU8_GLOBAL_TypeDef))/4];
    struct cc8_t:public CCU8_CC8_TypeDef {
	const uint32_t RESERVED[(256-sizeof(CCU8_CC8_TypeDef))/4];
    } cc[4];
    const uint32_t RESERVED2[3776];
};


////////////////////////////////////////////////////////////////////////////////
// Output pins
////////////////////////////////////////////////////////////////////////////////
template <int port, int pin, int unit, int slice, int output>
class out:public gpio::pin<port,pin> {
public:
    static constexpr int PORT=port;
    static constexpr int PIN=pin;
    static constexpr int UNIT=unit;
    static constexpr int SLICE=slice;
    static constexpr int OUTPUT=output;

    XMC_GPIO_MODE_t alt(void) {
	static_assert(port<0, "Not a CCU8 output");
	return XMC_GPIO_MODE_INPUT_TRISTATE;
    }

    void enable(XMC_GPIO_MODE_t i=XMC_GPIO_MODE_OUTPUT_PUSH_PULL) {
	this->set(XMC_GPIO_MODE_t(i|alt()));
#if (UC_DEVICE == XMC4400)
	this->set(XMC_GPIO_HWCTRL_DISABLED);
#endif
    }
};

#if UC_FAMILY == XMC4
extern ccu8_t dev[2];

template<> inline XMC_GPIO_MODE_t out<0,0,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,1,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,2,0,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,3,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,4,0,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,5,0,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,6,0,3,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,9,0,1,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,10,0,0,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<0,11,0,3,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,4,0,3,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,4,1,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<1,5,0,2,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,5,1,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<1,10,1,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,11,1,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,12,1,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,13,1,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,14,1,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<1,15,1,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,0,1,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out<2,1,1,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out<2,2,1,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out<2,6,0,1,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,7,0,0,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,8,0,3,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,9,0,2,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,14,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<2,15,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<5,0,1,3,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<5,1,1,3,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<5,2,1,2,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out<5,7,1,0,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
#endif

#if UC_FAMILY == XMC1

extern ccu8_t dev[1];

template<> inline XMC_GPIO_MODE_t out<0,0,0,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,1,0,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,2,0,0,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,2,0,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT7; }
template<> inline XMC_GPIO_MODE_t out<0,3,0,0,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,3,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT7; }
template<> inline XMC_GPIO_MODE_t out<0,4,0,1,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,5,0,1,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,5,0,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT7; }
template<> inline XMC_GPIO_MODE_t out<0,6,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,7,0,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,8,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,9,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,10,0,2,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,11,0,2,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,12,0,3,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,12,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT7; }
template<> inline XMC_GPIO_MODE_t out<0,13,0,3,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,13,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT7; }
template<> inline XMC_GPIO_MODE_t out<0,14,0,3,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<0,15,0,3,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,0,0,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,1,0,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,2,0,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,3,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,4,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<1,5,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<2,0,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
template<> inline XMC_GPIO_MODE_t out<2,1,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT5; }
#endif

////////////////////////////////////////////////////////////////////////////////
// Global functions
////////////////////////////////////////////////////////////////////////////////
// init(XMC_CCU8_CLOCK_SCU,XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR)
template <int UNIT>
void init(XMC_CCU8_CLOCK_t clock_source,XMC_CCU8_SLICE_MCMS_ACTION_t shadow_transfer) {
    static_assert(UNIT==0 || UNIT==1, "Only units 0 and 1 are valid");
    auto module=&dev[UNIT];

    auto gctrl = module->GCTRL;
    gctrl &= ~((uint32_t) CCU8_GCTRL_PCIS_Msk);
    gctrl |= bitfield<CCU8_GCTRL_PCIS_Msk>(clock_source);
    module->GCTRL = gctrl;

#if (UC_FAMILY == XMC4)
    /* Enable CCU8 module clock */
    XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_CCU);
#endif

#if defined(CLOCK_GATING_SUPPORTED)
    if constexpr (UNIT==0)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU80);
#ifdef CCU81
    if constexpr(UNIT==1)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU81);
#endif
#endif

#ifdef PERIPHERAL_RESET_SUPPORTED
    if constexpr (UNIT==0)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU80);
    else
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU81);
#endif

    module->GIDLC = (uint32_t) CCU8_GIDLC_SPRB_Msk;

    gctrl &= ~( CCU8_GCTRL_MSDE_Msk);
    gctrl |= bitfield<CCU8_GCTRL_MSDE_Msk>(shadow_transfer);
    module->GCTRL = gctrl;
}

enum edge_t {
    EDGE_NONE=0,
    EDGE_RISING=1,
    EDGE_FALLING=2,
    EDGE_BOTH=3
};

auto shadow_transfer=[](auto& ...x)
{
    if(uint32_t gcss=((x.UNIT==0 ? (1<<(4*x.SLICE)):0) | ...))
	dev[0].GCSS=gcss;
    if(uint32_t gcss=((x.UNIT==1 ? (1<<(4*x.SLICE)):0) | ...))
	dev[1].GCSS=gcss;
};

auto start=[](auto& ... x)
{
    if(uint32_t ccu80_gidlc=((x.UNIT==0 ? (1<<x.SLICE):0) | ...))
	dev[0].GIDLC=ccu80_gidlc | CCU8_GIDLC_SPRB_Msk;
    if(uint32_t ccu81_gidlc=((x.UNIT==1 ? (1<<x.SLICE):0) | ...))
	dev[1].GIDLC=ccu81_gidlc | CCU8_GIDLC_SPRB_Msk;

#if UC_FAMILY == XMC1
    constexpr uint32_t mask=SCU_GENERAL_CCUCON_GSC80_Msk;
#else
    uint32_t mask=(
	(x.UNIT? SCU_GENERAL_CCUCON_GSC81_Msk:SCU_GENERAL_CCUCON_GSC80_Msk)|...
    );
#endif
    SCU_GENERAL->CCUCON|=mask;
    SCU_GENERAL->CCUCON&=~mask;
};


////////////////////////////////////////////////////////////////////////////////
// Slice types
////////////////////////////////////////////////////////////////////////////////
template <int UNIT_PAR,int SLICE_PAR>
class slice_t {
public:
    static constexpr int UNIT=UNIT_PAR;
    static constexpr int SLICE=SLICE_PAR;
    CCU8_CC8_TypeDef *operator->(void) { return &dev[UNIT].cc[SLICE]; }

#if UC_FAMILY == XMC4
    template <int i> IRQn_Type irq(void) {
	static_assert(0<=i && i<=3, "Only four interrupts");
	if constexpr(UNIT==0) {
	    if constexpr (i==0) return CCU80_0_IRQn;
	    if constexpr (i==1) return CCU80_1_IRQn;
	    if constexpr (i==2) return CCU80_2_IRQn;
	    if constexpr (i==3) return CCU80_3_IRQn;
	}
	else {
	    if constexpr (i==0) return CCU81_0_IRQn;
	    if constexpr (i==1) return CCU81_1_IRQn;
	    if constexpr (i==2) return CCU81_2_IRQn;
	    if constexpr (i==3) return CCU81_3_IRQn;
	}
    }
#else
    template <int i> IRQn_Type irq(void) {
	static_assert(0<=i && i<=1, "Only two interrupts");
	if constexpr (i==0)
	    return CCU80_0_IRQn;
	return CCU80_1_IRQn;
    }
#endif
};

template <int UNIT_PAR, int SLICE_PAR, int OUTPUT_PAR>
class center_aligned:public slice_t<UNIT_PAR,SLICE_PAR> {
public:
    static constexpr int UNIT=UNIT_PAR;
    static constexpr int SLICE=SLICE_PAR;
    static constexpr int OUTPUT=OUTPUT_PAR;

    void init(void) {
	auto &cc=dev[UNIT].cc[SLICE];

	// Use GSC8x from SCU_GENERAL.CCUCON mapped to EVENT0 to start timer
	cc.INS=
	    bitfield<CCU8_CC8_INS_EV0IS_Msk>(CCU80_IN0_SCU_GSC80) |
	    bitfield<CCU8_CC8_INS_EV0EM_Msk>(EDGE_RISING);
	cc.CMC=bitfield<CCU8_CC8_CMC_STRTS_Msk>(1);
	cc.TCCLR=CCU8_CC8_TCCLR_TRBC_Msk | CCU8_CC8_TCCLR_TCC_Msk;

	cc.TC=CCU8_CC8_TC_TCM_Msk | // center aligned
	    CCU8_CC8_TC_CLST_Msk | // Shadow transfer on clear
	    CCU8_CC8_TC_STRM_Msk; // external start also clears timer
	cc.STC=0; // FIXME, not sure if this is best
    }
    void period(resolution_t t) {
	dev[UNIT].cc[SLICE].PRS=t.count()/2;
    }
    void operator=(float i) {
	if constexpr (OUTPUT==0 || OUTPUT==1)
	    dev[UNIT].cc[SLICE].CR1S=i*dev[UNIT].cc[SLICE].PRS;
	else
	    dev[UNIT].cc[SLICE].CR2S=i*dev[UNIT].cc[SLICE].PRS;
    }
};

template <typename HIGH, typename LOW>
class half_bridge:public center_aligned<HIGH::UNIT,HIGH::SLICE,HIGH::OUTPUT> {
public:
    static constexpr int UNIT=HIGH::UNIT;
    static constexpr int SLICE=HIGH::SLICE;
    static constexpr int OUTPUT=HIGH::OUTPUT;

    constexpr half_bridge(HIGH h, LOW l) {
	static_assert(std::is_same<
	    out<HIGH::PORT,HIGH::PIN,UNIT,SLICE,OUTPUT>,
	    HIGH>::value, "Pin must be a ccu8::out");
	static_assert(std::is_same<
	    out<LOW::PORT,LOW::PIN,UNIT,SLICE,OUTPUT-1>,
	    LOW>::value, "Pin must be a ccu8::out");
	static_assert(UNIT==0 || UNIT==1, "Oops");
	static_assert(h.UNIT==l.UNIT,
	    "Pins should belong to the same UNIT"
	);
	static_assert(HIGH::OUTPUT==(LOW::OUTPUT^1),
	    "Pins should be paired 0,1 or 2,3"
	);
	static_assert((LOW::OUTPUT&1)==0,
	    "Low side must be on out 0 or 2"
	);
    }

    void init(void) {
	center_aligned<UNIT,SLICE,OUTPUT>::init();
	HIGH{}.enable();
	LOW{}.enable();
    }
    void deadtime(resolution_t rising, resolution_t falling) {
	if constexpr(OUTPUT==0 || OUTPUT==1) {
	    dev[UNIT].cc[SLICE].DTC|= CCU8_CC8_DTC_DCEN2_Msk |
		CCU8_CC8_DTC_DCEN1_Msk | CCU8_CC8_DTC_DTE1_Msk;
	    dev[UNIT].cc[SLICE].DC1R=
		bitfield<CCU8_CC8_DC1R_DT1R_Msk>(rising.count()) |
		bitfield<CCU8_CC8_DC1R_DT1F_Msk>(falling.count());
	} else {
	    dev[UNIT].cc[SLICE].DTC|= CCU8_CC8_DTC_DCEN3_Msk |
		CCU8_CC8_DTC_DCEN4_Msk | CCU8_CC8_DTC_DTE2_Msk;
	    dev[UNIT].cc[SLICE].DC2R=
		bitfield<CCU8_CC8_DC2R_DT2R_Msk>(rising.count()) |
		bitfield<CCU8_CC8_DC2R_DT2F_Msk>(falling.count());
	}
    }
    void operator=(float i) {
	if constexpr (OUTPUT==0 || OUTPUT==1)
	    dev[UNIT].cc[SLICE].CR1S=i*dev[UNIT].cc[SLICE].PRS;
	else
	    dev[UNIT].cc[SLICE].CR2S=i*dev[UNIT].cc[SLICE].PRS;
    }
};

};
#endif
