#ifndef CCU8_H
#define CCU8_H

/*
    Figure 21-92 of the XMC4400 manual gives an overview of the registers.
    
*/

extern struct ccu8_t:public CCU8_GLOBAL_TypeDef {
    const uint32_t RESERVED2[31];
    struct cc8:public CCU8_CC8_TypeDef {
	const uint32_t RESERVED2[18];
    } cc[4];
} ccu8[2];

namespace ccu8_ns {
union GCTRL_t {
    struct {
	uint32_t PRBC:3;
	uint32_t const:1;
	uint32_t PCIS:2;
	uint32_t const:2;
	uint32_t SUSCFG:2;
	uint32_t MSE0:1;
	uint32_t MSE1:1;
	uint32_t MSE2:1;
	uint32_t MSE3:1;
	uint32_t MSDE:2;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union GIDLC_t {
    struct {
	uint32_t CC0:1;
	uint32_t CC1:1;
	uint32_t CC2:1;
	uint32_t CC3:1;
	uint32_t :4;
	uint32_t PRESCALER:1;
	uint32_t PRESCALER_CLEAR:1;
	uint32_t PARITY_CHECKER:1;
    } fields;
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union GSTAT_t {
    struct {
	uint32_t S0IDLE:1;
	uint32_t S1IDLE:1;
	uint32_t S2IDLE:1;
	uint32_t S3IDLE:1;
	uint32_t const:4;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

////////////////////////////////////////////////////////////////////////////////
// Slice registers
////////////////////////////////////////////////////////////////////////////////
union INS_t {
    struct {
	uint32_t EVENT0_INPUT:4; // Select which input to map to event 0
	uint32_t EVENT1_INPUT:4;
	uint32_t EVENT2_INPUT:4;
	uint32_t :4;
	uint32_t EVENT0_EDGE:2;	// 0=none,1=rising,2=falling,3=both
	uint32_t EVENT1_EDGE:2;
	uint32_t EVENT2_EDGE:2;
	uint32_t EVENT0_LEVEL:1; // 0=high level, 1=low level
	uint32_t EVENT1_LEVEL:1;
	uint32_t EVENT2_LEVEL:1;
	uint32_t EVENT0_LOW_PASS_FILTER:2; // 0=none, 1=3 cycles, 2=5, 3=7
	uint32_t EVENT1_LOW_PASS_FILTER:2;
	uint32_t EVENT2_LOW_PASS_FILTER:2;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union CMC_t {
    struct {
	uint32_t EXTERNAL_START:2; // 0=none, 1=event 0, 2=event 1, 3=event 2
	uint32_t EXTERNAL_STOP:2;
	uint32_t EXTERNAL_CAPTURE_0:2;
	uint32_t EXTERNAL_CAPTURE_1:2;
	uint32_t EXTERNAL_GATE:2;
	uint32_t EXTERNAL_UP_DOWN:2;
	uint32_t EXTERNAL_LOAD:2;
	uint32_t EXTERNAL_COUNT:2;
	uint32_t OVERRIDE:1;	// This field enables the ST bit override 
	uint32_t TRAP:1;	// trap functionality 0=disabled, 1=event 2
	uint32_t EXTERNAL_MODULATION:2;
	uint32_t CONCATENTATION:1; // disabled for cc[0]
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};


union TCST_t { // Slice Timer Status
    struct {
	uint32_t TIMER_RUN:1;
	uint32_t COUNTING_DIR:1; // 0=up, 1=down
	uint32_t :1;
	uint32_t DEAD_TIME_COUNTER1_RUN:1;
	uint32_t DEAD_TIME_COUNTER2_RUN:1;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union TCSET_t { // Slice Timer Run Set (start the timer)
    struct {
	uint32_t TIMER_START:1;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union TCCLR_t { // stop and clear the timer
    struct {
	uint32_t TIMER_STOP:1;
	uint32_t TIMER_CLEAR:1;
	uint32_t DITHER_CLEAR:1;
	uint32_t DEAD_TIME_1_CLEAR:1;
	uint32_t DEAD_TIME_2_CLEAR:1;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union TC_t {
    struct {
	uint32_t CENTER_ALIGNED:1; // Counting Mode (0=edge , 1=center aligned)
	uint32_t TSSM:1;	// Singleshot
	uint32_t CLST:1;	// Shadow Transfer on Clear
	uint32_t CMOD:1;	// Capture Compare Mode (0=compare,1=capture)
	uint32_t ECM:1;		// Extended Capture Mode
	uint32_t CAPC:2;	// Clear on Capture Control
	uint32_t TLS:1;		// Timer Load selector (0=CR1, 1=CR2)
	uint32_t ENDM:2;	// External Stop Function Control
	uint32_t STRM:2;	// External start Function Control
	uint32_t SCE:1;		// Equal Capture Event enable
	uint32_t CCS:1;		// Continuous Capture Enable
	uint32_t DITHE:2;	// Dither Enable
	uint32_t DIM:1;		// Dither input selector
	uint32_t FPE:1;		// Floating Prescaler enable
	uint32_t TRAPE0:1;	// TRAP enable for CCU8x.OUTy0
	uint32_t TRAPE1:1;	// TRAP enable for CCU8x.OUTy1
	uint32_t TRAPE2:1;	// TRAP enable for CCU8x.OUTy2
	uint32_t TRAPE3:1;	// TRAP enable for CCU8x.OUTy3
	uint32_t TRPSE:1;	// TRAP Synchronization Enable
	uint32_t TRPSW:1;	// TRAP State Clear Control
	uint32_t EMS:1;		// External Modulation Synchronization
	uint32_t EMT:1;		// External Modulation Type
	uint32_t MCME1:1;	// Multi Channel Mode Enable for Channel 1
	uint32_t MCME2:1;	// Multi Channel Mode Enable for Channel 2
	uint32_t EME1:1;	// External Modulation enable Channel 1
	uint32_t EME2:1;	// External Modulation enable Channel 2
	uint32_t STOS:2;	// Status bit output selector
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

// CC80PSL
// CC80DIT
// CC80PSC Prescaler Configuration


union INTE_t {
    struct {
	uint32_t PERIOD_MATCH:1;	// Period match while counting up
	uint32_t ONE_MATCH:1;		// One match while counting down 
	uint32_t COMPARE1_MATCH_UP:1;	// Ch1 Compare match while counting up 
	uint32_t COMPARE1_MATCH_DOWN:1;	// Ch1 Compare match while counting down
	uint32_t COMPARE2_MATCH_UP:1;	// Ch2 Compare match while counting up 
	uint32_t COMPARE2_MATCH_DOWN:1;	// Ch2 Compare match while counting down
	uint32_t :2;
	uint32_t EVENT0:1;		// Event 0 interrupt enable
	uint32_t EVENT1:1;		// Event 1 interrupt enable
	uint32_t EVENT2:1;		// Event 2 interrupt enable

    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union SRS_t {
    struct {
	uint32_t POSR:2;	// Period/One match Service request selector
	uint32_t CM1SR:2;	// Ch1 Compare match Service request selector
	uint32_t CM2SR:2;	// Ch2 Compare match Service request selector
	uint32_t :2;
	uint32_t E0SR:2;	// Event 0 Service request selector
	uint32_t E1SR:2;	// Event 1 Service request selector
	uint32_t E2SR:2;	// Event 2 Service request selector
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};


#define BASE__FUNCTION(name, type) 			\
template <int port, int pin> 				\
constexpr type name(iopin::pinBase<port,pin> const &i)	\
{ 							\
    static_assert(port==-1, "Invalid pin");		\
    return type(0);					\
}

#define SPECIALISATION(name, type, port, pin, value)		\
template<>							\
constexpr type name<port,pin>(iopin::pinBase<port,pin> const &i)	\
{								\
    return value;						\
}

BASE__FUNCTION(unit, int);
SPECIALISATION(unit, int, 0, 3, 0);
SPECIALISATION(unit, int, 0, 5, 0);
SPECIALISATION(unit, int, 0, 6, 0);

template <int port, int pin>
constexpr int unit(void) { static_assert(port==-1, "Invalid pin");}
template<> constexpr int unit<0,3>() { return 0; }
template<> constexpr int unit<0,5>() { return 0; }
template<> constexpr int unit<0,6>() { return 0; }

template <class T>
constexpr int unit(void) { return unit<T::PORT,T::PIN>(); }

BASE__FUNCTION(slice, int);
SPECIALISATION(slice, int, 0, 3, 2);
SPECIALISATION(slice, int, 0, 5, 0);
SPECIALISATION(slice, int, 0, 6, 3);

template <int port, int pin>
constexpr int slice(void) { static_assert(port==-1, "Invalid pin");}
template<> constexpr int slice<0,3>() { return 2; }
template<> constexpr int slice<0,5>() { return 0; }
template<> constexpr int slice<0,6>() { return 3; }

template <class T>
constexpr int slice(void) { return slice<T::PORT,T::PIN>(); }

BASE__FUNCTION(out, int);
SPECIALISATION(out, int, 0, 3, 0);
SPECIALISATION(out, int, 0, 5, 0);
SPECIALISATION(out, int, 0, 6, 0);

#undef BASE__FUNCTION
#undef SPECIALISATION

template <int u,int i> constexpr IRQn_Type irq() 
{ static_assert(u==-1,"Oops");}
template <> constexpr IRQn_Type irq<0,0>() { return CCU80_0_IRQn; }
template <> constexpr IRQn_Type irq<0,1>() { return CCU80_1_IRQn; }
template <> constexpr IRQn_Type irq<0,2>() { return CCU80_2_IRQn; }
template <> constexpr IRQn_Type irq<0,3>() { return CCU80_3_IRQn; }
template <> constexpr IRQn_Type irq<1,0>() { return CCU81_0_IRQn; }
template <> constexpr IRQn_Type irq<1,1>() { return CCU81_1_IRQn; }
template <> constexpr IRQn_Type irq<1,2>() { return CCU81_2_IRQn; }
template <> constexpr IRQn_Type irq<1,3>() { return CCU81_3_IRQn; }

template <int i, class T>
constexpr IRQn_Type irq(T const &o)
{
    return irq<unit(o),i>();
}

}

#endif
