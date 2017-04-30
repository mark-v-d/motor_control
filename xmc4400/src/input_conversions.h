#ifndef INPUT_CONVERSIONS_H
#define INPUT_CONVERSIONS_H

/*********
    FIXME, most of this file can be generated.
    POSIF is an exception. (but is complete for the XMC4400)
    FIXME, the 
*********/

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

namespace usic_ch_ns {
BASE__FUNCTION(unit, int);
SPECIALISATION(unit, int, 0,  0, 1);
SPECIALISATION(unit, int, 0,  4, 1);
SPECIALISATION(unit, int, 0,  5, 1);
SPECIALISATION(unit, int, 1,  4, 0);
SPECIALISATION(unit, int, 1,  5, 0);
SPECIALISATION(unit, int, 2,  2, 0);
SPECIALISATION(unit, int, 2,  5, 0);
SPECIALISATION(unit, int, 2, 14, 1);
SPECIALISATION(unit, int, 2, 15, 1);
SPECIALISATION(unit, int, 4,  0, 0);
SPECIALISATION(unit, int, 5,  0, 0);

BASE__FUNCTION(channel, int);
SPECIALISATION(channel, int, 0,  0, 1);
SPECIALISATION(channel, int, 0,  4, 0);
SPECIALISATION(channel, int, 0,  5, 0);
SPECIALISATION(channel, int, 1,  4, 0);
SPECIALISATION(channel, int, 1,  5, 0);
SPECIALISATION(channel, int, 2,  2, 1);
SPECIALISATION(channel, int, 2,  5, 1);
SPECIALISATION(channel, int, 2, 14, 0);
SPECIALISATION(channel, int, 2, 15, 0);
SPECIALISATION(channel, int, 4,  0, 1);
SPECIALISATION(channel, int, 5,  0, 0);

BASE__FUNCTION(dx0, int);
SPECIALISATION(dx0, int, 0,  0, USIC1_C1_DX0_P0_0);
SPECIALISATION(dx0, int, 0,  4, USIC1_C0_DX0_P0_4);
SPECIALISATION(dx0, int, 0,  5, USIC1_C0_DX0_P0_5);
SPECIALISATION(dx0, int, 1,  4, USIC0_C0_DX0_P1_4);
SPECIALISATION(dx0, int, 1,  5, USIC0_C0_DX0_P1_5);
SPECIALISATION(dx0, int, 2,  2, USIC0_C1_DX0_P2_2);
SPECIALISATION(dx0, int, 2,  5, USIC0_C1_DX0_P2_5);
//SPECIALISATION(dx0, int, 2, 14, USIC1_C0_DX0_P2_14);
//SPECIALISATION(dx0, int, 2, 15, USIC1_C0_DX0_P2_15);
//SPECIALISATION(dx0, int, 4,  0, USIC0_C1_DX0_P4_0);
//SPECIALISATION(dx0, int, 5,  0, USIC0_C0_DX0_P5_0);

BASE__FUNCTION(rxd, int);
SPECIALISATION(rxd, int, 1, 4, USIC0_C0_DX0_P1_4);
SPECIALISATION(rxd, int, 1, 5, USIC0_C0_DX0_P1_5);
#ifdef USIC0_C0_DX0_P5_0
SPECIALISATION(rxd_num, int, 5, 0, USIC0_C0_DX0_P5_0);
#endif

template <int port, int pin>
constexpr inline XMC_USIC_CH_t *xmc_channel(iopin::pinBase<port,pin> const &i)
{
    return unit(i)? 
	(channel(i)? XMC_UART1_CH1:XMC_UART1_CH0):
	(channel(i)? XMC_UART0_CH1:XMC_UART0_CH0);
}

template <int unit, int irq>
constexpr IRQn_Type irq_num(void) {
    static_assert(unit==-1, "unit or irq number not valid");
    return 0;
}

template<> constexpr IRQn_Type irq_num<0,0>(void) { return USIC0_0_IRQn; }
template<> constexpr IRQn_Type irq_num<0,1>(void) { return USIC0_1_IRQn; }
template<> constexpr IRQn_Type irq_num<0,2>(void) { return USIC0_2_IRQn; }
template<> constexpr IRQn_Type irq_num<0,3>(void) { return USIC0_3_IRQn; }
template<> constexpr IRQn_Type irq_num<0,4>(void) { return USIC0_4_IRQn; }
template<> constexpr IRQn_Type irq_num<0,5>(void) { return USIC0_5_IRQn; }
template<> constexpr IRQn_Type irq_num<1,0>(void) { return USIC1_0_IRQn; }
template<> constexpr IRQn_Type irq_num<1,1>(void) { return USIC1_1_IRQn; }
template<> constexpr IRQn_Type irq_num<1,2>(void) { return USIC1_2_IRQn; }
template<> constexpr IRQn_Type irq_num<1,3>(void) { return USIC1_3_IRQn; }
template<> constexpr IRQn_Type irq_num<1,4>(void) { return USIC1_4_IRQn; }
template<> constexpr IRQn_Type irq_num<1,5>(void) { return USIC1_5_IRQn; }

template <int innum,int port, int pin>
constexpr inline IRQn_Type irq(iopin::pinBase<port,pin> const &i)
{
    return irq_num<unit(i),innum>();
}

}

namespace posif_ns {
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
}

namespace dsd_ch_ns {
// Get channel number from MDAT pin
BASE__FUNCTION(channel, int);
SPECIALISATION(channel, int, 0, 7, 1);
SPECIALISATION(channel, int, 0, 8, 0);
SPECIALISATION(channel, int, 1, 5, 2);
SPECIALISATION(channel, int, 1, 6, 2);
SPECIALISATION(channel, int, 2, 6, 1);
SPECIALISATION(channel, int, 2, 7, 0);
SPECIALISATION(channel, int, 3, 3, 3);

// Get dsrc field value from MDAT pin
BASE__FUNCTION(dsrc, int);
SPECIALISATION(dsrc, int, 0, 7, 2);
SPECIALISATION(dsrc, int, 0, 8, 2);
SPECIALISATION(dsrc, int, 1, 5, 4);
SPECIALISATION(dsrc, int, 1, 6, 2);
SPECIALISATION(dsrc, int, 2, 6, 4);
SPECIALISATION(dsrc, int, 2, 7, 4);
SPECIALISATION(dsrc, int, 3, 3, 4);

// Get dsrc field value from inverted MDAT pin
BASE__FUNCTION(not_dsrc, int);
SPECIALISATION(not_dsrc, int, 0, 7, 3);
SPECIALISATION(not_dsrc, int, 0, 8, 3);
SPECIALISATION(not_dsrc, int, 1, 5, 5);
SPECIALISATION(not_dsrc, int, 1, 6, 3);
SPECIALISATION(not_dsrc, int, 2, 6, 5);
SPECIALISATION(not_dsrc, int, 2, 7, 5);
SPECIALISATION(not_dsrc, int, 3, 3, 5);
}

template <class T>
constexpr XMC_SCU_PERIPHERAL_CLOCK_t posif_clock(T const &o)
{
    return posif_unit(o)? XMC_SCU_PERIPHERAL_CLOCK_POSIF1:XMC_SCU_PERIPHERAL_CLOCK_POSIF0;
}

template <class T>
constexpr XMC_SCU_PERIPHERAL_RESET_t posif_reset(T const &o)
{
    return posif_unit(o)? XMC_SCU_PERIPHERAL_RESET_POSIF1:XMC_SCU_PERIPHERAL_RESET_POSIF0;
}

#undef BASE__FUNCTION
#undef SPECIALISATION

#endif
