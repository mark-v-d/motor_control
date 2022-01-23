#ifndef VADC_H
#define VADC_H
#include "xmc_vadc.h"

namespace adc {

static constexpr int group_count=XMC_VADC_MAXIMUM_NUM_GROUPS;

struct VADC_complete_t:public VADC_GLOBAL_TypeDef
{
    __I uint32_t RESERVED5[3];
    struct group:public VADC_G_TypeDef
    {
	__I uint32_t RESERVED[16];
    } G[group_count];
};

extern VADC_complete_t vadc;
extern SHS_Type shs0;

template <int group, int port, int pin>
constexpr int channel(void)
{
    static_assert(port==-1, "Invalid ADC pin for this group");
    return -1;
}

template <int group, int port, int pin>
constexpr int channel(gpio::pin<port,pin>const &)
{
    return channel<port,pin,group>();
}


#if UC_SERIES==XMC13
//  'a,s/G\(\d\+\)CH\(\d\+\).*\s\+I\s\+P\(\d\+\)\.\(\d\+\).*/template<> constexpr int channel<\2,\3,\1>(void0) { return \2; }
template<> constexpr int channel<2,0,0>(void) { return 5; }
template<> constexpr int channel<2,1,0>(void) { return 6; }
template<> constexpr int channel<2,2,0>(void) { return 7; }
template<> constexpr int channel<2,3,1>(void) { return 5; }
template<> constexpr int channel<2,4,1>(void) { return 6; }
template<> constexpr int channel<2,5,1>(void) { return 7; }
template<> constexpr int channel<2,6,0>(void) { return 0; }
template<> constexpr int channel<2,7,1>(void) { return 1; }
template<> constexpr int channel<2,8,0>(void) { return 1; }
template<> constexpr int channel<2,8,1>(void) { return 0; }
template<> constexpr int channel<2,9,0>(void) { return 2; }
template<> constexpr int channel<2,9,1>(void) { return 4; }
template<> constexpr int channel<2,10,0>(void) { return 3; }
template<> constexpr int channel<2,10,1>(void) { return 2; }
template<> constexpr int channel<2,11,0>(void) { return 4; }
template<> constexpr int channel<2,11,1>(void) { return 3; }
#elif UC_SERIES==XMC44
template<> constexpr int channel<0,14,0> { return 0; }
template<> constexpr int channel<1,14,0> { return 1; }
template<> constexpr int channel<2,14,0> { return 2; }
template<> constexpr int channel<3,14,0> { return 3; }
template<> constexpr int channel<4,14,0> { return 4; }
template<> constexpr int channel<5,14,0> { return 5; }
template<> constexpr int channel<6,14,0> { return 6; }
template<> constexpr int channel<7,14,0> { return 7; }
template<> constexpr int channel<0,14,1> { return 0; }
template<> constexpr int channel<1,14,1> { return 1; }
template<> constexpr int channel<2,14,1> { return 2; }
template<> constexpr int channel<3,14,1> { return 3; }
template<> constexpr int channel<4,14,1> { return 4; }
template<> constexpr int channel<5,14,1> { return 5; }
template<> constexpr int channel<6,14,1> { return 6; }
template<> constexpr int channel<7,14,1> { return 7; }
template<> constexpr int channel<0,14,2> { return 0; }
template<> constexpr int channel<1,14,2> { return 1; }
template<> constexpr int channel<2,15,2> { return 2; }
template<> constexpr int channel<3,15,2> { return 3; }
template<> constexpr int channel<0,15,3> { return 0; }
template<> constexpr int channel<1,15,3> { return 1; }
template<> constexpr int channel<2,14,3> { return 2; }
template<> constexpr int channel<3,14,3> { return 3; }
#endif

/*
    Write GxQINR0
    Trigger met GxQCTRL0
    Enable trigger met GxQMR0
    Enable request source met GxARBPR

    Trigger hardware/software GxQMR0.TREV = 1

    Refill


    Events ->

*/

////////////////////////////////////////////////////////////////////////////////
// Queue
////////////////////////////////////////////////////////////////////////////////
enum queue_attributes_t {
    REFILL=0x20,	// Reload in queue after sampling
    ENSI=0x40,		// Enable Source Interrupt
    EXTERNAL_TRIGGER=0x80,// Enable external trigger
};

template <int group,int PORT, int PIN>
void queue(gpio::pin<PORT,PIN> const &p, uint32_t attr)
{
    vadc.G[group].QINR0=channel<group>(p) | attr;
}

template <class T>
constexpr uint32_t queue_ctrl(T const &t) { return 0; }

template <class T>
constexpr uint32_t queue_qmr(T const &t) { return 0; }

template <>
constexpr uint32_t queue_qmr(XMC_VADC_GATEMODE_t const &t) {
    return bitfield<VADC_G_QMR0_ENGT_Msk>(t) | VADC_G_QCTRL0_GTWC_Msk;
}

template <>
constexpr uint32_t queue_ctrl(XMC_VADC_GATE_INPUT_SELECT_t const &t) {
    return bitfield<VADC_G_QCTRL0_GTSEL_Msk>(t) | VADC_G_QCTRL0_GTWC_Msk;
}

template <>
constexpr uint32_t queue_ctrl(XMC_VADC_TRIGGER_EDGE_t const &t) {
    return bitfield<VADC_G_QCTRL0_XTMODE_Msk>(t) | VADC_G_QCTRL0_XTWC_Msk;
}

template <>
constexpr uint32_t queue_ctrl(XMC_VADC_TRIGGER_INPUT_SELECT_t const &t) {
    return bitfield<VADC_G_QCTRL0_XTSEL_Msk>(t) | VADC_G_QCTRL0_XTWC_Msk;
}

template <>
constexpr uint32_t queue_ctrl(int const &t)
{
    return bitfield<VADC_G_QCTRL0_SRCRESREG_Msk>(t);
}

template <int group, typename ...TAIL>
void queue_config(TAIL const &...tail)
{
    static_assert(group<group_count, "Not that many groups");
    constexpr int trigger_input=
	(std::is_same<XMC_VADC_TRIGGER_INPUT_SELECT_t,TAIL>{} + ...);
    constexpr int trigger_edge=
	(std::is_same<XMC_VADC_TRIGGER_EDGE_t,TAIL>{} + ...);
    static_assert((trigger_input==1 && trigger_edge==1) ||
	(trigger_input==0 && trigger_edge==0),
	"Specify trigger level and input once or not at all");
    static_assert((std::is_same<int,TAIL>{} + ...)==1,
	"Always specify result destination");

    // FIXME, VADC_G_QCTRL0_TMEN_Msk missing

    vadc.G[group].QCTRL0=(queue_ctrl(tail) | ...);
    vadc.G[group].QMR0=(queue_qmr(tail) | ...)
	| (trigger_input* VADC_G_QMR0_ENTR_Msk);
}

////////////////////////////////////////////////////////////////////////////////
// Channel control
////////////////////////////////////////////////////////////////////////////////
inline uint32_t channel_control(XMC_VADC_CHANNEL_CONV_t const &x)
{
    return bitfield<VADC_G_CHCTR_ICLSEL_Msk>(x);
}

inline uint32_t channel_control(uint32_t const x)
{
    return bitfield<VADC_G_CHCTR_RESREG_Msk>(x);
}

template <int group, int PORT, int PIN, typename ...TAIL>
void channel_control(gpio::pin<PORT,PIN> const &p, TAIL const &...tail)
{
    uint32_t t=((channel_control(tail) | ...));
    vadc.G[group].CHCTR[channel<group>(p)]=t;
}

////////////////////////////////////////////////////////////////////////////////
// Conversion classes
////////////////////////////////////////////////////////////////////////////////
template <int cls>
void global_class(
    XMC_VADC_CONVMODE_t bits, uint32_t sample_time,
    XMC_VADC_CONVMODE_t emux_bits=XMC_VADC_CONVMODE_12BIT, uint32_t emux_time=0
) {
    static_assert(0<=cls && cls<=1, "There are two global conversion classes");
    vadc.GLOBICLASS[cls]=
	bitfield<VADC_GLOBICLASS_CMS_Msk>(bits)
	| bitfield<VADC_GLOBICLASS_STCS_Msk>(sample_time)
	| bitfield<VADC_GLOBICLASS_CMS_Msk>(emux_bits)
	| bitfield<VADC_GLOBICLASS_STCS_Msk>(emux_time);
}

void init(void)
{
    SCU_CLK->CGATCLR0=SCU_CLK_CGATCLR0_VADC_Msk;
    #if defined(PERIPHERAL_RESET_SUPPORTED)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_VADC);
    #endif
    XMC_VADC_GLOBAL_EnableModuleClock(&vadc);
    XMC_VADC_GLOBAL_StartupCalibration(&vadc);

    // Don't use compatible timing mode
    vadc.GLOBCFG=VADC_GLOBCFG_DIVWC_Msk
	| bitfield<VADC_GLOBCFG_DIVA_Msk>(0)
	| bitfield<VADC_GLOBCFG_DIVD_Msk>(0);

    // Reference voltage? pg 16-17
}

}

#endif
