#ifndef CCU4_H
#define CCU4_H

#include "gpio.h"
#include "xmc_ccu4.h"
#include <chrono>

namespace ccu4 {

typedef std::chrono::duration<float,std::ratio<1,FREQUENCY>> resolution_t;

struct ccu4_t:public CCU4_GLOBAL_TypeDef {
    const uint32_t RESERVED[(256-sizeof(CCU4_GLOBAL_TypeDef))/4];
    struct cc4_t:public CCU4_CC4_TypeDef {
	const uint32_t RESERVED[(256-sizeof(CCU4_CC4_TypeDef))/4];
    } cc[4];
    const uint32_t RESERVED2[3776];
};


////////////////////////////////////////////////////////////////////////////////
// Output pins
////////////////////////////////////////////////////////////////////////////////
struct par_t {
    int UNIT, SLICE;
    XMC_GPIO_MODE_t mode;
};

template <int port,int pin>
constexpr par_t alt(void) {
    static_assert(port<0, "Not a CCU4 output");
    return par_t{-2,-2,XMC_GPIO_MODE_OUTPUT_ALT1};
}

#if UC_FAMILY == XMC4
extern ccu4_t dev[3];
extern ccu4_t dev3;

template<> constexpr par_t alt<0,12>(void){ return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<1,0>(void) { return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<1,1>(void) { return par_t{0,2,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<1,2>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<1,3>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<2,2>(void) { return par_t{1,3,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<2,3>(void) { return par_t{1,2,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<2,4>(void) { return par_t{1,1,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<2,5>(void) { return par_t{1,0,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<3,0>(void) { return par_t{2,0,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<3,3>(void) { return par_t{2,3,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<3,4>(void) { return par_t{2,2,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<3,5>(void) { return par_t{2,1,XMC_GPIO_MODE_OUTPUT_ALT3}; }
template<> constexpr par_t alt<3,6>(void) { return par_t{2,0,XMC_GPIO_MODE_OUTPUT_ALT3}; }

#endif

#if UC_FAMILY == XMC1

extern ccu4_t dev[1];
extern ccu4_t dev3;

template<> constexpr par_t alt<0,0>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,1>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,2>(void) { return par_t{0,2,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,3>(void) { return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,4>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,5>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,6>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,7>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,8>(void) { return par_t{0,2,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,9>(void) { return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<1,0>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<1,1>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<1,2>(void) { return par_t{0,2,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<1,3>(void) { return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<2,0>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<2,1>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<2,10>(void) { return par_t{0,2,XMC_GPIO_MODE_OUTPUT_ALT2}; }
template<> constexpr par_t alt<2,11>(void) { return par_t{0,3,XMC_GPIO_MODE_OUTPUT_ALT2}; }

template <int UNIT, int INPUT, int PORT, int PIN>
uint32_t get_event(gpio::pin<PORT,PIN>)
{
    static_assert(PORT<0, "Not a valid input event");
    return 0;
}


template <> inline uint32_t get_event<0,0,0,0>(gpio::pin<0,0>) { return CCU40_IN0_P0_0; }
#ifdef CCU40_IN0_P0_12
template <> inline uint32_t get_event<0,0,0,12>(gpio::pin<0,12>) { return CCU40_IN0_P0_12; }
#endif
template <> inline uint32_t get_event<0,0,0,6>(gpio::pin<0,6>) { return CCU40_IN0_P0_6; }

#ifdef CCU40_IN1_P0_1
template <> inline uint32_t get_event<0,1,0,1>(gpio::pin<0,1>) { return CCU40_IN1_P0_1;}
#endif
#ifdef CCU40_IN1_P0_12
template <> inline uint32_t get_event<0,1,0,12>(gpio::pin<0,12>) { return CCU40_IN1_P0_12;}
#endif
#ifdef CCU40_IN1_P0_7
template <> inline uint32_t get_event<0,1,0,7>(gpio::pin<0,7>) { return CCU40_IN1_P0_7;}
#endif

#ifdef CCU40_IN2_P0_12
template <> inline uint32_t get_event<0,2,0,12>(gpio::pin<0,12>) { return CCU40_IN2_P0_12; }
#endif
#ifdef CCU40_IN2_P0_2
template <> inline uint32_t get_event<0,2,0,2>(gpio::pin<0,2>) { return CCU40_IN2_P0_2; }
#endif
template <> inline uint32_t get_event<0,2,0,8>(gpio::pin<0,8>) { return CCU40_IN2_P0_8; }

#ifdef CCU40_IN3_P0_12
template <> inline uint32_t get_event<0,3,0,12>(gpio::pin<0,12>) { return CCU40_IN3_P0_12; }
#endif
#ifdef CCU40_IN3_P0_3
template <> inline uint32_t get_event<0,3,0,3>(gpio::pin<0,3>) { return CCU40_IN3_P0_3; }
#endif
template <> inline uint32_t get_event<0,3,0,9>(gpio::pin<0,9>) { return CCU40_IN3_P0_9; }
#endif

template <int port, int pin>
class out:public gpio::pin<port,pin> {
public:
    static constexpr int PORT=port;
    static constexpr int PIN=pin;
    static constexpr int SLICE=alt<port,pin>().SLICE;
    static constexpr int OUTPUT=alt<port,pin>().OUTPUT;;

    void enable(XMC_GPIO_MODE_t i=XMC_GPIO_MODE_OUTPUT_PUSH_PULL) {
	this->set(XMC_GPIO_MODE_t(i|alt<port,pin>().mode));
#if (UC_DEVICE == XMC4400)
	this->set(XMC_GPIO_HWCTRL_DISABLED);
#endif
    }
};


////////////////////////////////////////////////////////////////////////////////
// Global functions
////////////////////////////////////////////////////////////////////////////////
// init(XMC_CCU4_CLOCK_SCU,XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR)
template <int UNIT>
void init(XMC_CCU4_CLOCK_t clock_source,
    XMC_CCU4_SLICE_MCMS_ACTION_t shadow_transfer
) {
    static_assert(UNIT<=3 || UNIT>=0, "Only units 0 to 3 are valid");
    auto module=UNIT==3? &dev3:&dev[UNIT];

    auto gctrl = module->GCTRL;
    gctrl &= ~((uint32_t) CCU4_GCTRL_PCIS_Msk);
    gctrl |= bitfield<CCU4_GCTRL_PCIS_Msk>(clock_source);
    module->GCTRL = gctrl;

#if (UC_FAMILY == XMC4)
    /* Enable CCU4 module clock */
    XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_CCU);
#endif

#if defined(CLOCK_GATING_SUPPORTED)
    if constexpr (UNIT==0)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU40);
#ifdef CCU41
    if constexpr(UNIT==1)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU41);
#endif
#ifdef CCU42
    if constexpr(UNIT==2)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU42);
#endif
#ifdef CCU43
    if constexpr(UNIT==3)
	XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_CCU43);
#endif
#endif

#ifdef PERIPHERAL_RESET_SUPPORTED
    if constexpr (UNIT==0)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU40);
    if constexpr (UNIT==1)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU41);
    if constexpr (UNIT==2)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU42);
    if constexpr (UNIT==3)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_CCU43);
#endif

    module->GIDLC = (uint32_t) CCU4_GIDLC_SPRB_Msk;

    gctrl &= ~( CCU4_GCTRL_MSDE_Msk);
    gctrl |= bitfield<CCU4_GCTRL_MSDE_Msk>(shadow_transfer);
    module->GCTRL = gctrl;
}

enum edge_t {
    EDGE_NONE=0,
    EDGE_RISING=1,
    EDGE_FALLING=2,
    EDGE_BOTH=3
};

enum level_t {
    LEVEL_HIGH=0,
    LEVEL_LOW=1
};

inline auto shadow_transfer=[](auto& ...x)
{
    if(uint32_t gcss=((x.UNIT==0 ? (1<<(4*x.SLICE)):0) | ...))
	dev[0].GCSS=gcss;
    if(uint32_t gcss=((x.UNIT==1 ? (1<<(4*x.SLICE)):0) | ...))
	dev[1].GCSS=gcss;
    if(uint32_t gcss=((x.UNIT==2 ? (1<<(4*x.SLICE)):0) | ...))
	dev[2].GCSS=gcss;
    if(uint32_t gcss=((x.UNIT==3 ? (1<<(4*x.SLICE)):0) | ...))
	dev3.GCSS=gcss;
};

inline auto start=[](auto& ... x)
{
    // FIXME, create combined CCU4/CCU8 start
    if(uint32_t ccu40_gidlc=((x.UNIT==0 ? (1<<x.SLICE):0) | ...))
	dev[0].GIDLC=ccu40_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu41_gidlc=((x.UNIT==1 ? (1<<x.SLICE):0) | ...))
	dev[1].GIDLC=ccu41_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu42_gidlc=((x.UNIT==2 ? (1<<x.SLICE):0) | ...))
	dev[2].GIDLC=ccu42_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu43_gidlc=((x.UNIT==2 ? (1<<x.SLICE):0) | ...))
	dev3.GIDLC=ccu43_gidlc | CCU4_GIDLC_SPRB_Msk;

#if UC_FAMILY == XMC1
    constexpr uint32_t mask=SCU_GENERAL_CCUCON_GSC40_Msk;
#else
    uint32_t mask=(
	(x.UNIT? SCU_GENERAL_CCUCON_GSC41_Msk:SCU_GENERAL_CCUCON_GSC40_Msk)|...
    );
#endif
    SCU_GENERAL->CCUCON|=mask;
    SCU_GENERAL->CCUCON&=~mask;
};

inline auto stop=[](auto& ... x)
{
    if(uint32_t ccu40_gidls=((x.UNIT==0 ? (1<<x.SLICE):0) | ...))
	dev[0].GIDLS=ccu40_gidls;
    if(uint32_t ccu41_gidls=((x.UNIT==1 ? (1<<x.SLICE):0) | ...))
	dev[1].GIDLS=ccu41_gidls;
    if(uint32_t ccu42_gidls=((x.UNIT==2 ? (1<<x.SLICE):0) | ...))
	dev[2].GIDLS=ccu42_gidls;
    if(uint32_t ccu43_gidls=((x.UNIT==3 ? (1<<x.SLICE):0) | ...))
	dev3.GIDLS=ccu43_gidls;
};


////////////////////////////////////////////////////////////////////////////////
// Slice types
////////////////////////////////////////////////////////////////////////////////
template <int UNIT_PAR,int SLICE_PAR>
class slice_t {
public:
    static constexpr int UNIT=UNIT_PAR;
    static constexpr int SLICE=SLICE_PAR;
    static constexpr int TYPE=4;

    CCU4_GLOBAL_TypeDef *global() {
	if constexpr (UNIT==3)
	    return &dev3;
	else
	    return &dev[UNIT];
    }
    CCU4_CC4_TypeDef *addr() {
	if constexpr (UNIT==3)
	    return &dev3.cc[SLICE];
	else
	    return &dev[UNIT].cc[SLICE];
    }

    auto operator->() {
	return addr();
    }

#if UC_FAMILY == XMC4
    template <int i> IRQn_Type irq(void) {
	static_assert(0<=i && i<=3, "Only four interrupts");
	if constexpr(UNIT==0) {
	    if constexpr (i==0) return CCU40_0_IRQn;
	    if constexpr (i==1) return CCU40_1_IRQn;
	    if constexpr (i==2) return CCU40_2_IRQn;
	    if constexpr (i==3) return CCU40_3_IRQn;
	}
	if constexpr(UNIT==1){
	    if constexpr (i==0) return CCU41_0_IRQn;
	    if constexpr (i==1) return CCU41_1_IRQn;
	    if constexpr (i==2) return CCU41_2_IRQn;
	    if constexpr (i==3) return CCU41_3_IRQn;
	}
	if constexpr(UNIT==2){
	    if constexpr (i==0) return CCU42_0_IRQn;
	    if constexpr (i==1) return CCU42_1_IRQn;
	    if constexpr (i==2) return CCU42_2_IRQn;
	    if constexpr (i==3) return CCU42_3_IRQn;
	}
	if constexpr(UNIT==3){
	    if constexpr (i==0) return CCU43_0_IRQn;
	    if constexpr (i==1) return CCU43_1_IRQn;
	    if constexpr (i==2) return CCU43_2_IRQn;
	    if constexpr (i==3) return CCU43_3_IRQn;
	}
    }
#else
    template <int i> IRQn_Type irq(void) {
	static_assert(0<=i && i<=3, "Only four interrupts");
	if constexpr (i==0)
	    return CCU40_0_IRQn;
	if constexpr (i==1)
	    return CCU40_1_IRQn;
	if constexpr (i==2)
	    return CCU40_2_IRQn;
	if constexpr (i==3)
	    return CCU40_3_IRQn;
    }
#endif
    void init(void) {
	auto &cc=*this->addr();

	// Use GSC4x from SCU_GENERAL.CCUCON mapped to EVENT0 to start timer
	cc.INS=
	    bitfield<CCU4_CC4_INS_EV0IS_Msk>(CCU40_IN0_SCU_GSC40) |
	    bitfield<CCU4_CC4_INS_EV0EM_Msk>(EDGE_RISING);
	cc.CMC=bitfield<CCU4_CC4_CMC_STRTS_Msk>(1);
	cc.TCCLR=CCU4_CC4_TCCLR_TRBC_Msk | CCU4_CC4_TCCLR_TCC_Msk;

	cc.TC=
	    CCU4_CC4_TC_TCM_Msk |
	    CCU4_CC4_TC_CLST_Msk | // Shadow transfer on clear
	    CCU4_CC4_TC_STRM_Msk; // external start also clears timer
    }

    void start() {
	global()->GIDLC=1<<SLICE;
	addr()->TCSET=CCU4_CC4_TCSET_TRBS_Msk;
    }

    template <int EVENT>
    void set_event(int event,edge_t edge,
	level_t level=LEVEL_HIGH, int low_pass=0
    ) {
	static_assert(0<=EVENT && EVENT<=2, "Only events 0,1,2 exist");
	auto ins=addr()->INS;
	constexpr uint32_t input_mask=(CCU4_CC4_INS_EV0IS_Msk<<(4*EVENT));
	constexpr uint32_t edge_mask=(CCU4_CC4_INS_EV0EM_Msk<<(2*EVENT));
	constexpr uint32_t level_mask=(CCU4_CC4_INS_EV0LM_Msk<<(EVENT));
	constexpr uint32_t lowpass_mask=(CCU4_CC4_INS_LPF0M_Msk<<(2*EVENT));
	ins&=~(input_mask | edge_mask | level_mask | lowpass_mask);
	ins|=bitfield<input_mask>(event)
	    | bitfield<edge_mask>(edge)
	    | bitfield<level_mask>(level)
	    | bitfield<lowpass_mask>(low_pass);
	addr()->INS=ins;
    }

    template <int EVENT, int PORT, int PIN>
    void set_event(gpio::pin<PORT,PIN> pin,edge_t edge,
	level_t level=LEVEL_HIGH, int low_pass=0
    ) {
	set_event<EVENT>(get_event<UNIT,SLICE>(pin),edge,level,low_pass);
    }

    template <int EVENT>
    void service_request_event(int request_line) {
	auto srs=addr()->SRS;
	srs&=~(CCU4_CC4_SRS_E0SR_Msk<<(2*EVENT));
	srs|=bitfield<CCU4_CC4_SRS_E0SR_Msk<<(2*EVENT)>(request_line);
	addr()->SRS=srs;
	// just assume we want to enable... otherwise why configure it?
	addr()->INTE|=(CCU4_CC4_INTE_E0AE_Msk<<EVENT);
    }

    template <int EVENT>
    void disable_service_request_event(void) {
	addr()->INTE&=~(CCU4_CC4_INTE_E0AE_Msk<<EVENT);
    }

    template <int CAPTURE>
    void enable_capture(int event) {
	static_assert(0<=CAPTURE && CAPTURE<=1, "Only capture 0 and 1 exist");
	auto cmc=addr()->CMC;
	constexpr uint32_t mask=CCU4_CC4_CMC_CAP0S_Msk<<(CAPTURE*2);
	cmc&=~mask;
	cmc|=bitfield<mask>(event+1);
	addr()->CMC=cmc;
    }
    template <int CAPTURE>
    void disable_capture(void) {
	static_assert(0<=CAPTURE && CAPTURE<=1, "Only capture 0 and 1 exist");
	addr()->CMC&=~(CCU4_CC4_CMC_CAP0S_Msk<<(CAPTURE*2));
    }

    void period(resolution_t t) {
	addr()->PRS=t.count();
    }
    void operator=(float i) {
	this->addr()->CRS=i*this->addr()->PRS;
    }
    void center_period(resolution_t t) {
	addr()->PRS=t.count()/2;
    }
};

template <int UNIT_PAR, int SLICE_PAR>
class edge_capture:public slice_t<UNIT_PAR,SLICE_PAR> {
public:
    static constexpr int UNIT=UNIT_PAR;
    static constexpr int SLICE=SLICE_PAR;
};

template <int port, int pin>
class center_aligned:public slice_t<alt<port,pin>().UNIT,alt<port,pin>().SLICE> {
public:
    static constexpr int UNIT=alt<port,pin>().UNIT;
    static constexpr int SLICE=alt<port,pin>().SLICE;

    constexpr center_aligned(gpio::pin<port,pin> x) {}

    void init(void) {
	auto &cc=*this->addr();
	out<port,pin>().enable();

	slice_t<UNIT,SLICE>::init();

	cc.TC=CCU4_CC4_TC_TCM_Msk | // center aligned
	    CCU4_CC4_TC_CLST_Msk | // Shadow transfer on clear
	    CCU4_CC4_TC_STRM_Msk; // external start also clears timer
    }
    void period(resolution_t t) {
	this->addr()->PRS=t.count()/2;
    }
    void operator=(float i) {
	this->addr()->CRS=i*this->addr()->PRS;
    }
};

};
#endif
