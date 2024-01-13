#ifndef POSIF_H
#define POSIF_H
#include "ccu4.h"

namespace posif {

struct POSIF_PADDED_t:public POSIF_GLOBAL_TypeDef {
  __I  uint32_t  RESERVED6[4031];
};

extern POSIF_PADDED_t dev[2];


#define BASE__FUNCTION(name, type) 			\
template <int port, int pin> 				\
constexpr type name(gpio::pin<port,pin> const &i)	\
{ 							\
    static_assert(port==-1, "Invalid pin");		\
    return type(0);					\
}

#define SPECIALISATION(name, type, port, Pin, value)		\
template<>							\
constexpr type name<port,Pin>(gpio::pin<port,Pin> const &i)	\
{								\
    return value;						\
}

BASE__FUNCTION(unit, int);
SPECIALISATION(unit, int, 1, 1, 0); 	// posif 0A
SPECIALISATION(unit, int, 1, 2, 0);
SPECIALISATION(unit, int, 1, 3, 0);
SPECIALISATION(unit, int, 14, 5, 0);	// posif 0B
SPECIALISATION(unit, int, 14, 6, 0);
SPECIALISATION(unit, int, 14, 7, 0);
SPECIALISATION(unit, int, 2, 3, 1); 	// posif 1A
SPECIALISATION(unit, int, 2, 4, 1);
SPECIALISATION(unit, int, 2, 5, 1);

BASE__FUNCTION(pinZ, int);
BASE__FUNCTION(pinB, int);
BASE__FUNCTION(pinA, int);
SPECIALISATION(pinZ, int,  1, 1, 0);	// posif 0A
SPECIALISATION(pinB, int,  1, 2, 0);
SPECIALISATION(pinA, int,  1, 3, 0);
SPECIALISATION(pinZ, int, 14, 5, 1);	// posif 0B
SPECIALISATION(pinB, int, 14, 6, 1);
SPECIALISATION(pinA, int, 14, 7, 1);
SPECIALISATION(pinZ, int,  2, 3, 0);	// posif 1A
SPECIALISATION(pinB, int,  2, 4, 0);
SPECIALISATION(pinA, int,  2, 5, 0);

template <int UNIT>
inline void init() {
    if constexpr(UNIT==0) {
#if defined(CLOCK_GATING_SUPPORTED)
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF0);
#endif
#if defined(PERIPHERAL_RESET_SUPPORTED)
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF0);
#endif
    }
#if defined(POSIF1)
    if constexpr(UNIT==1) {
#if defined(CLOCK_GATING_SUPPORTED)
      XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF1);
#endif
#if defined(PERIPHERAL_RESET_SUPPORTED)
     XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF1);
#endif
    }
#endif
}

/*******************************************************************************
    Base class for using POSIF
*******************************************************************************/
// Quadrature without index, using a 32-bit counter.
template <typename enc_a, typename enc_b>
class qd32_t {
public:
    static constexpr int UNIT=unit(enc_a{});
    using cnt_l=ccu4::slice_t<UNIT,0>;
    using cnt_h=ccu4::slice_t<UNIT,1>;
    static constexpr POSIF_PADDED_t *module=&dev[UNIT];

    qd32_t() {
	enc_a A;
	enc_b B;
	static_assert(unit(A)==unit(B), "A,B should be same posif unit");
#ifdef POSIF1
	static_assert(CCU40_IN0_POSIF0_OUT0==CCU41_IN0_POSIF1_OUT0, "Oops");
	static_assert(CCU40_IN0_POSIF0_OUT1==CCU41_IN0_POSIF1_OUT1, "Oops");
	static_assert(CCU40_IN0_POSIF0_OUT3==CCU41_IN0_POSIF1_OUT3, "Oops");
	static_assert(CCU40_IN1_POSIF0_OUT0==CCU41_IN1_POSIF1_OUT0, "Oops");
	static_assert(CCU40_IN1_POSIF0_OUT1==CCU41_IN1_POSIF1_OUT1, "Oops");
	static_assert(CCU40_IN1_POSIF0_OUT3==CCU41_IN1_POSIF1_OUT3, "Oops");
	static_assert(CCU40_IN1_POSIF0_OUT4==CCU41_IN1_POSIF1_OUT4, "Oops");
	// static_assert(CCU40_IN2_POSIF0_OUT1==CCU41_IN2_POSIF1_OUT1, "Oops");
	static_assert(CCU40_IN2_POSIF0_OUT2==CCU41_IN2_POSIF1_OUT2, "Oops");
	static_assert(CCU40_IN2_POSIF0_OUT3==CCU41_IN2_POSIF1_OUT3, "Oops");
	static_assert(CCU40_IN2_POSIF0_OUT4==CCU41_IN2_POSIF1_OUT4, "Oops");
	static_assert(CCU40_IN3_POSIF0_OUT3==CCU41_IN3_POSIF1_OUT3, "Oops");
	static_assert(CCU40_IN3_POSIF0_OUT5==CCU41_IN3_POSIF1_OUT5, "Oops");
#else
	static_assert(unit(A)==0, "POSIF1 does not exist");
#endif
    };
    qd32_t(enc_a A, enc_b B):qd32_t() { }

    void init(int reverse=0) {
	enc_a A; A.set(XMC_GPIO_MODE_INPUT_TRISTATE); A.input_enable();
	enc_b B; B.set(XMC_GPIO_MODE_INPUT_TRISTATE); B.input_enable();
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_CCU);
	if(UNIT==0) {
	 XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF0);
	 XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF0);
	} else {
	 XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF1);
	 XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF1);
	}

	module->PCONF=bitfield<POSIF_PCONF_FSEL_Msk>(1)
	    | bitfield<POSIF_PCONF_INSEL0_Msk>(pinA(A))
	    | bitfield<POSIF_PCONF_INSEL1_Msk>(pinB(B));
	module->QDC=bitfield<POSIF_QDC_ICM_Msk>(2)
	    | bitfield<POSIF_QDC_PHS_Msk>(reverse);
	module->PRUNS=1;

	ccu4::init<UNIT>(
	    XMC_CCU4_CLOCK_SCU,
	    XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR
	);
	using namespace ccu4;
	cnt_l l;
	l.template set_event<0>(CCU40_IN0_POSIF0_OUT0,EDGE_RISING); // count
	l.template set_event<1>(CCU40_IN1_POSIF0_OUT1,EDGE_RISING); // up/down
	l.template set_event<2>(CCU40_IN2_POSIF0_OUT3,EDGE_RISING); // index
	l->CMC=bitfield<CCU4_CC4_CMC_CNTS_Msk>(1)
	    | bitfield<CCU4_CC4_CMC_UDS_Msk>(2)
	    | bitfield<CCU4_CC4_CMC_CAP0S_Msk>(3);
	l->PRS=0xffff;

	cnt_h h;
	h.template set_event<0>(CCU40_IN0_POSIF0_OUT0,EDGE_RISING); // count
	h.template set_event<1>(CCU40_IN1_POSIF0_OUT1,EDGE_RISING); // up/down
	h.template set_event<2>(CCU40_IN2_POSIF0_OUT3,EDGE_RISING); // index
	h->CMC=bitfield<CCU4_CC4_CMC_CNTS_Msk>(1)
	    | bitfield<CCU4_CC4_CMC_UDS_Msk>(2)
	    | bitfield<CCU4_CC4_CMC_CAP0S_Msk>(3)
	    | CCU4_CC4_CMC_TCE_Msk;
	h->PRS=0xffff;

	h.start();
	l.start();
	ccu4::start(l,h);
	ccu4::shadow_transfer(l,h);
    }

    int32_t count() { return cnt_l{}->TIMER | (cnt_h{}->TIMER<<16); }
    void setcount(int32_t c) {
	cnt_l L;
	cnt_h H;
	L->TCCLR=1; L->TIMER=c&0xffff; L->TCSET=1;
	H->TCCLR=1; H->TIMER=c>>16; H->TCSET=1;
    }
    //int32_t index() { return (0xffff&cnt_l{}->CV[1]) | (cnt_h{}->CV[1]<<16); }
};


// Quadrature with index, using a 32-bit counter.
template <typename enc_a, typename enc_b, typename enc_z>
class qdi32_t:qd32_t<enc_a,enc_b> {
public:
    using base_t=qd32_t<enc_a,enc_b>;
    using base_t::UNIT;
    using base_t::module;
    using cnt_l=ccu4::slice_t<UNIT,0>;
    using cnt_h=ccu4::slice_t<UNIT,1>;

    qdi32_t():base_t() {
	enc_z Z;
	static_assert(unit(Z)==UNIT, "A,B,Z should be same posif unit");
    };
    qdi32_t(enc_a A, enc_b B, enc_z Z):qdi32_t() { }

    void init() {
	base_t::init();
	enc_z Z; Z.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	Z.input_enable();
	module->PCONF|=bitfield<POSIF_PCONF_INSEL2_Msk>(pinZ(Z));
    }

    void negative_index() {
	enc_z{}.set(XMC_GPIO_MODE_INPUT_INVERTED_TRISTATE);
    }

    void positive_index() {
	enc_z{}.set(XMC_GPIO_MODE_INPUT_TRISTATE);
    }

    using base_t::count;
    int32_t index() {
	cnt_l L;
	cnt_h H;
	return (0xffff&L->CV[1]) | (H->CV[1]<<16);
    }
};


}
#endif
