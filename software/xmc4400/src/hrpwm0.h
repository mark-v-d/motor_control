#ifndef HRPWM0_H
#define HRPWM0_H

#include <xmc_hrpwm.h>
#include <cmath>
#include "ccu8.h"

namespace hrpwm0 {

struct HRPWM0_HRC_Type_padded:public HRPWM0_HRC_Type {
    uint32_t RESERVED[50];
};

extern HRPWM0_Type dev;
extern HRPWM0_HRC_Type_padded hrc[4];

////////////////////////////////////////////////////////////////////////////////
// Output pins
////////////////////////////////////////////////////////////////////////////////
struct par_t {
    int SLICE, OUTPUT;
    XMC_GPIO_MODE_t mode;
};

template <int port,int pin>
constexpr par_t alt(void) {
    static_assert(port<0, "Not a HRPWM0 output");
    return par_t{-2,-2,XMC_GPIO_MODE_OUTPUT_ALT1};
}
template<> constexpr par_t alt<0,2>(void) { return par_t{0,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,3>(void) { return par_t{2,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,4>(void) { return par_t{2,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,5>(void) { return par_t{0,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,6>(void) { return par_t{3,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,7>(void) { return par_t{1,1,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,8>(void) { return par_t{1,0,XMC_GPIO_MODE_OUTPUT_ALT4}; }
template<> constexpr par_t alt<0,9>(void) { return par_t{3,1,XMC_GPIO_MODE_OUTPUT_ALT1}; }

template <int port, int pin>
class out:public gpio::pin<port,pin> {
public:
    static constexpr int PORT=port;
    static constexpr int PIN=pin;
    static constexpr int SLICE=alt<port,pin>().SLICE;
    static constexpr int OUTPUT=alt<port,pin>().OUTPUT;;

    constexpr out() {}
    constexpr out(gpio::pin<port,pin>) {}

    void enable(XMC_GPIO_MODE_t i=XMC_GPIO_MODE_OUTPUT_PUSH_PULL) {
	this->set(XMC_GPIO_MODE_t(i|alt<port,pin>().mode));
	this->set(XMC_GPIO_HWCTRL_DISABLED);
    }
};

/* Execute before hrpwm0::init
    ccu8::init<test_out.UNIT>(
	XMC_CCU8_CLOCK_SCU,
	XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR
    );
*/
inline auto init(void)
{
     auto status=XMC_HRPWM_Init(&dev);
     XMC_HRPWM_EnableGlobalHR(&dev);
     return XMC_HRPWM_GetHRGenReadyStatus(&dev);
     //return status;
}


template <int SLICE>
struct hrc_registers_t {
    HRPWM0_HRC_Type *operator->(void) { return &hrc[SLICE]; }
};

template <int UNIT,int SLICE, int OUTPUT>
struct all_registers_t {
    ccu8::slice_t<UNIT,SLICE> ccu8;
    hrc_registers_t<SLICE> hrc;
};


template <typename HIGH, typename LOW>
class half_bridge:public ccu8::center_aligned<0,HIGH::SLICE,0> {
    using base_t=ccu8::center_aligned<0,HIGH::SLICE,0>;
public:
    static constexpr int SLICE=HIGH::SLICE;
    static constexpr int OUTPUT=0; // ccu8 output
    static constexpr int UNIT=0; // ccu8 unit

    using base_t::set_trap;
    using base_t::clear_trap;

    constexpr half_bridge() {
	HIGH h{};
	LOW l{};
	static_assert(std::is_same<out<HIGH::PORT,HIGH::PIN>, HIGH>::value,
	    "Pin must be a hrpwm0::out");
	static_assert(std::is_same<out<LOW::PORT,LOW::PIN>, LOW>::value,
	    "Pin must be a hrpwm0::out");
	static_assert(h.SLICE==l.SLICE,
	    "Pins should belong to the same SLICE"
	);
    }

    constexpr half_bridge(HIGH h, LOW l):half_bridge() { }

    auto operator ->(void) {
	static all_registers_t<UNIT,SLICE,OUTPUT> x;
	return &x;
    }

    void deadtime(ccu8::resolution_t rising, ccu8::resolution_t falling) {
	hrc[SLICE].SDCR=rising.count();
	hrc[SLICE].SDCF=falling.count();
    }

    void init(bool invert_h, bool invert_l) {
	ccu8::center_aligned<UNIT,SLICE,OUTPUT>::init();
	dev.HRCCFG|=(HRPWM0_HRCCFG_HRC0E_Msk<<SLICE);

	auto hr=&hrc[SLICE];
	hr->GC|=HRPWM0_HRC_GC_STC_Msk|HRPWM0_HRC_GC_DSTC_Msk
	    | HRPWM0_HRC_GC_DTE_Msk
	    | bitfield<HRPWM0_HRC_GC_HRM0_Msk>(2);
	hr->PL=(invert_h? 2:0)|(invert_l? 1:0);
	hr->GSEL=
	    bitfield<HRPWM0_HRC_GSEL_S0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_C0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_S0ES_Msk>(1) | // rising edge
	    bitfield<HRPWM0_HRC_GSEL_C0ES_Msk>(2); // falling edge
	hr->TSEL=
	    HRPWM0_HRC_TSEL_TS0E_Msk |
	    bitfield<HRPWM0_HRC_TSEL_TSEL0_Msk>(SLICE);

	HIGH{}.enable();
	LOW{}.enable();
    }

    float operator=(float i) {
	using namespace std::chrono_literals;

	float b=ccu8::dev[UNIT].cc[SLICE].PRS*i+1.5f;
	if constexpr (OUTPUT==0 || OUTPUT==1)
	    ccu8::dev[UNIT].cc[SLICE].CR1S=std::floor(b);
	else
	    ccu8::dev[UNIT].cc[SLICE].CR2S=std::floor(b);
	b-=std::floor(b);
	constexpr int factor=ccu8::resolution_t(1)/0.15ns;
	hrc[SLICE].SCR1=factor*b;
	hrc[SLICE].SCR2=factor*(1.0f-b)-0.5f;
	return i;
    }
    /* Met dithering,  (pagina 2198)
	PRS_pseudo=PRS+DCV/16
	timing iedere pwm cycle aanpassen?
    */

    void enable_trap() {
	auto hr=&hrc[SLICE];
	hr->GC|=HRPWM0_HRC_GC_TR0E_Msk | HRPWM0_HRC_GC_TR1E_Msk;
    }

    void disable_trap() {
	auto hr=&hrc[SLICE];
	hr->GC&=~(HRPWM0_HRC_GC_TR0E_Msk | HRPWM0_HRC_GC_TR1E_Msk);
    }
};

template <typename HIGH>
class center_aligned:public ccu8::center_aligned<0,HIGH::SLICE,0> {
    using base_t=ccu8::center_aligned<0,HIGH::SLICE,0>;
public:
    static constexpr int SLICE=HIGH::SLICE;
    static constexpr int OUTPUT=0; // ccu8 output
    static constexpr int UNIT=0; // ccu8 unit

    using base_t::set_trap;
    using base_t::clear_trap;

    constexpr center_aligned() {
	HIGH h{};
	static_assert(std::is_same<out<HIGH::PORT,HIGH::PIN>, HIGH>::value,
	    "Pin must be a hrpwm0::out");
    }

    constexpr center_aligned(HIGH h):center_aligned() { }

    auto operator ->(void) {
	static all_registers_t<UNIT,SLICE,OUTPUT> x;
	return &x;
    }

    void deadtime(ccu8::resolution_t rising, ccu8::resolution_t falling) {
	hrc[SLICE].SDCR=rising.count();
	hrc[SLICE].SDCF=falling.count();
    }

    void init(bool invert_h=false) {
	ccu8::center_aligned<UNIT,SLICE,OUTPUT>::init();
	dev.HRCCFG|=(HRPWM0_HRCCFG_HRC0E_Msk<<SLICE);

	auto hr=&hrc[SLICE];
	hr->GC|=HRPWM0_HRC_GC_STC_Msk|HRPWM0_HRC_GC_DSTC_Msk
	    | HRPWM0_HRC_GC_DTE_Msk
	    | bitfield<HRPWM0_HRC_GC_HRM0_Msk>(2);
	hr->PL=invert_h? 2:0;
	hr->GSEL=
	    bitfield<HRPWM0_HRC_GSEL_S0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_C0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_S0ES_Msk>(1) | // rising edge
	    bitfield<HRPWM0_HRC_GSEL_C0ES_Msk>(2); // falling edge
	hr->TSEL=
	    HRPWM0_HRC_TSEL_TS0E_Msk |
	    bitfield<HRPWM0_HRC_TSEL_TSEL0_Msk>(SLICE);

	HIGH{}.enable();
    }

    float operator=(float i) {
	using namespace std::chrono_literals;

	float b=ccu8::dev[UNIT].cc[SLICE].PRS*i+1.5f;
	if constexpr (OUTPUT==0 || OUTPUT==1)
	    ccu8::dev[UNIT].cc[SLICE].CR1S=std::floor(b);
	else
	    ccu8::dev[UNIT].cc[SLICE].CR2S=std::floor(b);
	b-=std::floor(b);
	constexpr int factor=ccu8::resolution_t(1)/0.15ns;
	hrc[SLICE].SCR1=factor*b;
	hrc[SLICE].SCR2=factor*(1.0f-b)-0.5f;
	return i;
    }
    /* Met dithering,  (pagina 2198)
	PRS_pseudo=PRS+DCV/16
	timing iedere pwm cycle aanpassen?
    */

    void enable_trap() {
	auto hr=&hrc[SLICE];
	hr->GC|=HRPWM0_HRC_GC_TR0E_Msk | HRPWM0_HRC_GC_TR1E_Msk;
    }

    void disable_trap() {
	auto hr=&hrc[SLICE];
	hr->GC&=~(HRPWM0_HRC_GC_TR0E_Msk | HRPWM0_HRC_GC_TR1E_Msk);
    }
};


}

#endif
