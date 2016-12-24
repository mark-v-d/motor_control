#ifndef INPUT_CONVERSIONS_H
#define INPUT_CONVERSIONS_H

/*********
    FIXME, most of this file can be generated.
    POSIF is an exception.
    FIXME, the 
*********/

#define BASE__FUNCTION(name, type) 			\
template <int port, int pin> 				\
constexpr type name(iopin::input<port,pin> const &i)	\
{ 							\
    static_assert(port==-1, "Invalid pin");		\
    return type(0);					\
}

#define SPECIALISATION(name, type, port, pin, value)		\
template<>							\
constexpr type name<port,pin>(iopin::input<port,pin> const &i)	\
{								\
    return value;						\
}

BASE__FUNCTION(rxd_num, int);
SPECIALISATION(rxd_num, int, 1, 4, USIC0_C0_DX0_P1_4);
SPECIALISATION(rxd_num, int, 1, 5, USIC0_C0_DX0_P1_5);
#ifdef USIC0_C0_DX0_P5_0
SPECIALISATION(rxd_num, int, 5, 0, USIC0_C0_DX0_P5_0);
#endif

BASE__FUNCTION(posif_unit, int);
SPECIALISATION(posif_unit, int, 14, 6, 0);
SPECIALISATION(posif_unit, int, 14, 7, 0);

BASE__FUNCTION(posif_pinZ, int);
BASE__FUNCTION(posif_pinB, int);
BASE__FUNCTION(posif_pinA, int);
SPECIALISATION(posif_pinZ, int,  2, 3, 0);
SPECIALISATION(posif_pinB, int,  2, 4, 0);
SPECIALISATION(posif_pinA, int,  2, 5, 0);
SPECIALISATION(posif_pinZ, int, 14, 5, 1);
SPECIALISATION(posif_pinB, int, 14, 6, 1);
SPECIALISATION(posif_pinA, int, 14, 7, 1);


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
