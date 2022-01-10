#ifndef CCU8_H
#define CCU8_H

#include "gpio.h"
#include <chrono>

namespace ccu8 {

typedef std::chrono::duration<float,std::ratio<1,FREQUENCY>> resolution_t;

struct ccu8_t:public CCU8_GLOBAL_TypeDef {
    const uint32_t RESERVED[31];
    struct cc8_t:public CCU8_CC8_TypeDef {
	const uint32_t RESERVED[18];
    } cc[4];
    const uint32_t RESERVED2[3776];
};

extern ccu8_t dev[2];

////////////////////////////////////////////////////////////////////////////////
// Output pins
////////////////////////////////////////////////////////////////////////////////
template <int port, int pin, int unit, int slice, int output>
class out_t:public gpio::pin<port,pin> {
public:
    static constexpr int UNIT=unit;
    static constexpr int SLICE=slice;
    static constexpr int OUTPUT=output;

    XMC_GPIO_MODE_t alt(void) {
	static_assert(port<0, "Not a CCU8 output");
	return XMC_GPIO_MODE_INPUT_TRISTATE;
    }

    void enable(XMC_GPIO_MODE_t i=XMC_GPIO_MODE_OUTPUT_PUSH_PULL) {
	this->set(XMC_GPIO_MODE_t(i|alt()));
	//set(XMC_GPIO_HWCTRL_DISABLED);
    }
};

template<> inline XMC_GPIO_MODE_t out_t<0,0,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,1,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,2,0,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,3,0,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,4,0,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,5,0,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,6,0,3,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,9,0,1,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,10,0,0,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<0,11,0,3,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,4,0,3,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,4,1,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out_t<1,5,0,2,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,5,1,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out_t<1,10,1,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,11,1,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,12,1,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,13,1,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,14,1,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<1,15,1,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,0,1,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out_t<2,1,1,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out_t<2,2,1,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT2; }
template<> inline XMC_GPIO_MODE_t out_t<2,6,0,1,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,7,0,0,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,8,0,3,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,9,0,2,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,14,0,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<2,15,0,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<5,0,1,3,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<5,1,1,3,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<5,2,1,2,3>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }
template<> inline XMC_GPIO_MODE_t out_t<5,7,1,0,2>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT3; }

////////////////////////////////////////////////////////////////////////////////
// Global functions
////////////////////////////////////////////////////////////////////////////////
// init(XMC_CCU8_CLOCK_SCU,XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR)
template <int UNIT>
void init(XMC_CCU8_CLOCK_t clock_source,XMC_CCU8_SLICE_MCMS_ACTION_t shadow_transfer) {
    static_assert(UNIT==0 || UNIT==1, "Only units 0 and 1 are valid");
    auto &module=dev[UNIT];

    auto xmc=reinterpret_cast<XMC_CCU8_MODULE_t*>(&module);
    XMC_CCU8_SetModuleClock(xmc, XMC_CCU8_CLOCK_SCU);
    XMC_CCU8_EnableModule(xmc);
    XMC_CCU8_Init(xmc, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    return;

    auto gctrl=module.GCTRL;
    gctrl&=~CCU8_GCTRL_PCIS_Msk;
    gctrl|=bitfield<CCU8_GCTRL_PCIS_Msk>(clock_source);
    module.GCTRL=gctrl;

    if constexpr(UNIT) {
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU81);
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU81);
    } else {
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU80);
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU80);
    }
    // Start prescaler
    module.GIDLC = (uint32_t) CCU8_GIDLC_SPRB_Msk;

    gctrl&=~CCU8_GCTRL_MSDE_Msk;
    gctrl|=bitfield<CCU8_GCTRL_MSDE_Msk>(XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    module.GCTRL=gctrl;
}

enum edge_t {
    EDGE_NONE=0,
    EDGE_RISING=1,
    EDGE_FALLING=2,
    EDGE_BOTH=3
};

template <typename ...Args>
void shadow_transfer(Args &...x)
{
    if(uint32_t gcss=((x.UNIT==0 ? (1<<(4*x.SLICE)):0) | ...))
	dev[0].GCSS=gcss;
    if(uint32_t gcss=((x.UNIT==1 ? (1<<(4*x.SLICE)):0) | ...))
	dev[1].GCSS=gcss;
}

template <typename ...Args>
void start(Args &...x)
{
    if(uint32_t ccu80_gidlc=((x.UNIT==0 ? (1<<x.SLICE):0) | ...))
	dev[0].GIDLC=ccu80_gidlc | CCU8_GIDLC_SPRB_Msk;
    if(uint32_t ccu81_gidlc=((x.UNIT==1 ? (1<<x.SLICE):0) | ...))
	dev[1].GIDLC=ccu81_gidlc | CCU8_GIDLC_SPRB_Msk;

    uint32_t mask=(
	(x.UNIT? SCU_GENERAL_CCUCON_GSC81_Msk:SCU_GENERAL_CCUCON_GSC80_Msk)|...
    );
    SCU_GENERAL->CCUCON|=mask;
    SCU_GENERAL->CCUCON&=~mask;
}


////////////////////////////////////////////////////////////////////////////////
// Slice types
////////////////////////////////////////////////////////////////////////////////
template <int UNIT_PAR,int SLICE_PAR>
class slice_t {
public:
    static constexpr int UNIT=UNIT_PAR;
    static constexpr int SLICE=SLICE_PAR;
    CCU8_CC8_TypeDef *operator->(void) { return &dev[UNIT].cc[SLICE]; }

    template <int i> IRQn_Type irq(void) {
	static_assert(0<=i && i<=3, "Only three interrupts");
	if constexpr(UNIT==0) {
	    if constexpr (i==0) return CCU80_0_IRQn;
	    if constexpr (i==1) return CCU80_1_IRQn;
	    if constexpr (i==2) return CCU80_2_IRQn;
	    if constexpr (i==3) return CCU80_3_IRQn;
	} else {
	    if constexpr (i==0) return CCU81_0_IRQn;
	    if constexpr (i==1) return CCU81_1_IRQn;
	    if constexpr (i==2) return CCU81_2_IRQn;
	    if constexpr (i==3) return CCU81_3_IRQn;
	}
    }
};

template <typename HIGH, typename LOW>
class half_bridge:public slice_t<HIGH::UNIT,HIGH::SLICE> {
public:
    static constexpr int UNIT=HIGH::UNIT;
    static constexpr int SLICE=HIGH::SLICE;
    static constexpr int OUTPUT=HIGH::OUTPUT;

    constexpr half_bridge(HIGH h, LOW l) {
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
	auto &cc=dev[UNIT].cc[SLICE];

	// Use GSC8x from SCU_GENERAL.CCUCON mapped to EVENT0 to start timer
	cc.INS=
	    bitfield<CCU8_CC8_INS_EV0IS_Msk>(7) |
	    bitfield<CCU8_CC8_INS_EV0EM_Msk>(EDGE_RISING);
	cc.CMC=bitfield<CCU8_CC8_CMC_STRTS_Msk>(1);
	cc.TCCLR=CCU8_CC8_TCCLR_TRBC_Msk | CCU8_CC8_TCCLR_TCC_Msk;

	cc.TC=CCU8_CC8_TC_TCM_Msk | // center aligned
	    CCU8_CC8_TC_CLST_Msk | // Shadow transfer on clear
	    CCU8_CC8_TC_STRM_Msk; // external start also clears timer
	cc.STC=0; // FIXME, not sure if this is best

	// We cannot invert anyway, since the deadtime will not work
	cc.PSL&=~((1<<LOW::OUTPUT)|(1<<HIGH::OUTPUT));
	cc.CHC&=~((2<<LOW::OUTPUT)|(2<<LOW::OUTPUT));

	HIGH{}.enable();
	LOW{}.enable();
    }
    void period(resolution_t t) {
	dev[UNIT].cc[SLICE].PRS=t.count()/2;
    }
    void deadtime(resolution_t t) {
	if constexpr(OUTPUT==0 || OUTPUT==1) {
	    dev[UNIT].cc[SLICE].DTC|= CCU8_CC8_DTC_DCEN2_Msk |
		CCU8_CC8_DTC_DCEN1_Msk | CCU8_CC8_DTC_DTE1_Msk;
	    dev[UNIT].cc[SLICE].DC1R=
		bitfield<CCU8_CC8_DC1R_DT1R_Msk>(t.count()) |
		bitfield<CCU8_CC8_DC1R_DT1F_Msk>(t.count());
	} else {
	    dev[UNIT].cc[SLICE].DTC|= CCU8_CC8_DTC_DCEN3_Msk |
		CCU8_CC8_DTC_DCEN4_Msk | CCU8_CC8_DTC_DTE2_Msk;
	    dev[UNIT].cc[SLICE].DC2R=
		bitfield<CCU8_CC8_DC2R_DT2R_Msk>(t.count()) |
		bitfield<CCU8_CC8_DC2R_DT2F_Msk>(t.count());
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
