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

// Quadrature with index, using a 32-bit counter.
template <typename enc_a, typename enc_b, typename enc_z>
class qdi32_t {
public:
    static constexpr int UNIT=unit(enc_a{});
    using cnt_l=ccu4::slice_t<UNIT,0>;
    using cnt_h=ccu4::slice_t<UNIT,1>;
    static constexpr POSIF_PADDED_t *module=&dev[UNIT];

    qdi32_t(enc_a A, enc_b B, enc_z Z) {
	static_assert(unit(A)==unit(B) && unit(A)==unit(Z),
	    "A,B,Z should be same posif unit");
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

    void init() {
	enc_a A; A.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	enc_a B; B.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	enc_a Z; Z.set(XMC_GPIO_MODE_INPUT_TRISTATE); 

	module->PCONF=bitfield<POSIF_PCONF_FSEL_Msk>(1)
	    | bitfield<POSIF_PCONF_INSEL0_Msk>(pinA(A))
	    | bitfield<POSIF_PCONF_INSEL1_Msk>(pinB(B))
	    | bitfield<POSIF_PCONF_INSEL2_Msk>(pinZ(Z))
	    | bitfield<POSIF_PCONF_LPC_Msk>(3);
	module->QDC=bitfield<POSIF_QDC_ICM_Msk>(2);
	module->PRUNS=1;

	using namespace ccu4;
	ccu4::init<UNIT>();
	cnt_l l;
	cnt_h h;
	l.template set_event<0>(CCU40_IN0_POSIF0_OUT0,EDGE_RISING); // count
	l.template set_event<1>(CCU40_IN1_POSIF0_OUT1,EDGE_RISING); // up/down
	l.template set_event<2>(CCU40_IN2_POSIF0_OUT3,EDGE_RISING); // index
	l->CMC=bitfield<CCU4_CC4_CMC_CNTS_Msk>(1)
	    | bitfield<CCU4_CC4_CMC_UDS_Msk>(2)
	    | bitfield<CCU4_CC4_CMC_CAP0S_Msk>(3);

	h->CMC=CCU4_CC4_CMC_TCE_Msk;

	l.start();
    }

    int32_t count() { return cnt_l{}->TIMER | (cnt_h{}->TIMER<<16); }
    int32_t index() { return (0xffff&cnt_l{}->CV[1]) | (cnt_h{}->CV[1]<<16); }
};


}
#endif
