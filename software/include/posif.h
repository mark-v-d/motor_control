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


// every encoder encountered sofar, does not work with the index from posif
SPECIALISATION(unit, int,  2, 1, 0);
SPECIALISATION(unit, int,  2, 8, 0);
SPECIALISATION(pinZ, int,  1, 3, 16+CCU40_IN0_P1_3);
SPECIALISATION(pinZ, int,  2, 1, 16+CCU40_IN0_P2_1);
SPECIALISATION(pinZ, int,  2, 8, 16+CCU40_IN0_P2_8);

SPECIALISATION(unit, int,  1, 4, 1);
SPECIALISATION(unit, int,  2, 9, 1);
SPECIALISATION(pinZ, int,  1, 4, 16+CCU41_IN0_P1_4);
SPECIALISATION(pinZ, int,  2, 5, 16+CCU41_IN0_P2_5);
SPECIALISATION(pinZ, int,  2, 9, 16+CCU41_IN0_P2_9);

BASE__FUNCTION(pinCaptureSlice2, int);
SPECIALISATION(pinCaptureSlice2, int, 1, 1,  CCU40_IN2_P1_1);
SPECIALISATION(pinCaptureSlice2, int, 2, 3,  CCU41_IN2_P2_3);

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
    using cnt_i=ccu4::slice_t<UNIT,2>;
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

    void init(int reverse=0, int Z=0) {
	enc_a A; A.set(XMC_GPIO_MODE_INPUT_TRISTATE); A.input_enable();
	enc_b B; B.set(XMC_GPIO_MODE_INPUT_TRISTATE); B.input_enable();
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_CCU);
	if(UNIT==0) {
	 XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF0);
	 XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF0);
	 XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF0);
	} else {
	 XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF1);
	 XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_POSIF1);
	 XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_POSIF1);
	}

	module->PCONF=bitfield<POSIF_PCONF_FSEL_Msk>(1)
	    | bitfield<POSIF_PCONF_INSEL0_Msk>(pinA(A))
	    | bitfield<POSIF_PCONF_INSEL1_Msk>(pinB(B))
	    | bitfield<POSIF_PCONF_INSEL2_Msk>(Z);
	module->QDC=bitfield<POSIF_QDC_ICM_Msk>(2)
	    | bitfield<POSIF_QDC_PHS_Msk>(reverse);

	ccu4::init<UNIT>(
	    XMC_CCU4_CLOCK_SCU,
	    XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR
	);
	using namespace ccu4;
	cnt_l l;
	cnt_h h;
	cnt_i i;
	l.template set_event<0>(CCU40_IN0_POSIF0_OUT0,EDGE_RISING);
	l.template set_event<1>(CCU40_IN0_POSIF0_OUT1,EDGE_RISING);
	if(Z<16)
	    l.template set_event<2>(CCU40_IN0_POSIF0_OUT3,EDGE_RISING);
	else
	    l.template set_event<2>(Z-16,EDGE_RISING);
	l->CMC=bitfield<CCU4_CC4_CMC_CNTS_Msk>(1)
	    | bitfield<CCU4_CC4_CMC_UDS_Msk>(2)
	    | bitfield<CCU4_CC4_CMC_CAP0S_Msk>(3);
	l->PRS=0xffff;

	h.template set_event<0>(CCU40_IN1_POSIF0_OUT0,EDGE_RISING);
	h.template set_event<1>(CCU40_IN1_POSIF0_OUT1,EDGE_RISING);
	h->CMC=bitfield<CCU4_CC4_CMC_CNTS_Msk>(1)
	    | bitfield<CCU4_CC4_CMC_UDS_Msk>(2)
	    | bitfield<CCU4_CC4_CMC_CAP0S_Msk>(3)
	    | CCU4_CC4_CMC_TCE_Msk;
	h->PRS=0xffff;

	module->PRUNS=1;
	l.start(); h.start(); i.start();
	ccu4::start(l,h,i);
	ccu4::shadow_transfer(l,h,i);

	// Hack to start counting, not sure why this is necessary
	A.set(XMC_GPIO_MODE_INPUT_INVERTED_TRISTATE);
	for(int i=0; i<100; i++)
	    asm volatile("nop");
	A.set(XMC_GPIO_MODE_INPUT_TRISTATE);
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
    using typename base_t::cnt_l;
    using typename base_t::cnt_h;
    using typename base_t::cnt_i;

    qdi32_t():base_t() {
	enc_z Z;
	static_assert(unit(Z)==UNIT, "A,B,Z should be same posif unit");
    };
    qdi32_t(enc_a A, enc_b B, enc_z Z):qdi32_t() { }

    void init() {
	enc_z Z; Z.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	Z.input_enable();
	base_t::init(0,pinZ(Z));
    }

    void negative_index() {
	enc_z{}.set(XMC_GPIO_MODE_INPUT_INVERTED_TRISTATE);
    }

    void positive_index() {
	enc_z{}.set(XMC_GPIO_MODE_INPUT_TRISTATE);
    }

    void skip_posif_index() {
	static_assert(pinZ(enc_z{})<16, "POSIF already skipped");
	using namespace ccu4;

	if(UNIT==0)
	    cnt_l{}.template set_event<2>(CCU40_IN0_CCU40_ST2,EDGE_RISING);
	else
	    cnt_l{}.template set_event<2>(CCU41_IN0_CCU41_ST2,EDGE_RISING);

	cnt_i i;
	i.template set_event<0>(pinCaptureSlice2(enc_z{}),EDGE_RISING);
	i->TC=CCU4_CC4_TC_TSSM_Msk;			// single shot mode
	i->CMC=bitfield<CCU4_CC4_CMC_STRTS_Msk>(1); // use event0 to start timer
	i->PRS=2;
	i->CRS=1;

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
