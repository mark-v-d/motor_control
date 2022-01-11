#ifndef HRPWM0_H
#define HRPWM0_H

#include <xmc_hrpwm.h>
#include <cmath>
#include "ccu8.h"

namespace hrpwm0 {

struct HRPWM0_HRC_Type_padded:public HRPWM0_HRC_Type {
    uint32_t RESERVED[50];
};

struct CSG_GLOBAL_t {
  __IO uint32_t  HRBSC;                             /*!< (@ 0x40020900) Bias and suspend configuration                         */
  __I  uint32_t  RESERVED;
  __I  uint32_t  MIDR;                              /*!< (@ 0x40020908) Module identification register                         */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  GLBANA;                            /*!< (@ 0x40020914) Global Analog Configuration                            */
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  CSGCFG;                            /*!< (@ 0x40020920) Global CSG configuration                               */
  __O  uint32_t  CSGSETG;                           /*!< (@ 0x40020924) Global CSG run bit set                                 */
  __O  uint32_t  CSGCLRG;                           /*!< (@ 0x40020928) Global CSG run bit clear                               */
  __I  uint32_t  CSGSTATG;                          /*!< (@ 0x4002092C) Global CSG run bit status                              */
  __O  uint32_t  CSGFCG;                            /*!< (@ 0x40020930) Global CSG slope/prescaler control                     */
  __I  uint32_t  CSGFSG;                            /*!< (@ 0x40020934) Global CSG slope/prescaler status                      */
  __O  uint32_t  CSGTRG;                            /*!< (@ 0x40020938) Global CSG shadow/switch trigger                       */
  __O  uint32_t  CSGTRC;                            /*!< (@ 0x4002093C) Global CSG shadow trigger clear                        */
  __I  uint32_t  CSGTRSG;                           /*!< (@ 0x40020940) Global CSG shadow/switch status                        */
};

struct HRC_GLOBAL_t {
  __IO uint32_t  HRCCFG;                            /*!< (@ 0x40020960) Global HRC configuration                               */
  __O  uint32_t  HRCSTRG;                           /*!< (@ 0x40020964) Global HRC shadow trigger set                          */
  __O  uint32_t  HRCCTRG;                           /*!< (@ 0x40020968) Global HRC shadow trigger clear                        */
  __I  uint32_t  HRCSTSG;                           /*!< (@ 0x4002096C) Global HRC shadow transfer status                      */
  __I  uint32_t  HRGHRS;                            /*!< (@ 0x40020970) High Resolution Generation Status                      */
};

extern CSG_GLOBAL_t csg_global;
extern HRC_GLOBAL_t hrc_global;

extern HRPWM0_Type dev;
extern HRPWM0_HRC_Type hrc[4];

////////////////////////////////////////////////////////////////////////////////
// Output pins
////////////////////////////////////////////////////////////////////////////////
template <int port, int pin, int slice, int output>
class out:public gpio::pin<port,pin> {
public:
    static constexpr int PORT=port;
    static constexpr int PIN=pin;
    static constexpr int SLICE=slice;
    static constexpr int OUTPUT=output;

    XMC_GPIO_MODE_t alt(void) {
	static_assert(port<0, "Not a HRPWM0 output");
	return XMC_GPIO_MODE_INPUT_TRISTATE;
    }

    void enable(XMC_GPIO_MODE_t i=XMC_GPIO_MODE_OUTPUT_PUSH_PULL) {
	this->set(XMC_GPIO_MODE_t(i|alt()));
	this->set(XMC_GPIO_HWCTRL_DISABLED);
    }
};

template<> inline XMC_GPIO_MODE_t out<0,2,0,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,3,2,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,4,2,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,5,0,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,6,3,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,7,1,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,8,1,0>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT4; }
template<> inline XMC_GPIO_MODE_t out<0,9,3,1>::alt(void) { return XMC_GPIO_MODE_OUTPUT_ALT1; }

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

template <typename HIGH, typename LOW>
class half_bridge:public ccu8::center_aligned<0,HIGH::SLICE,0> {
public:
    static constexpr int SLICE=HIGH::SLICE;
    static constexpr int OUTPUT=0; // ccu8 output
    static constexpr int UNIT=0; // ccu8 unit

    constexpr half_bridge(HIGH h, LOW l) {
	static_assert(std::is_same<
	    out<HIGH::PORT,HIGH::PIN,SLICE,HIGH::OUTPUT>,
	    HIGH>::value, "Pin must be a hrpwm0::out");
	static_assert(std::is_same<
	    out<LOW::PORT,LOW::PIN,SLICE,LOW::OUTPUT>,
	    LOW>::value, "Pin must be a hrpwm0::out");
	static_assert(h.SLICE==l.SLICE,
	    "Pins should belong to the same SLICE"
	);
    }

    // HRPWM0_HRC_Type* operator->(void) { return &hrc[UNIT]; }

    void deadtime(ccu8::resolution_t rising, ccu8::resolution_t falling) {
	hrc[SLICE].SDCR=rising.count();
	hrc[SLICE].SDCF=falling.count();
    }

    void init(bool invert_h, bool invert_l) {
	ccu8::center_aligned<UNIT,SLICE,OUTPUT>::init();
	dev.HRCCFG|=0x10;

	auto hr=&hrc[SLICE];
	hr->GSEL=
	    bitfield<HRPWM0_HRC_GSEL_S0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_C0M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_S0ES_Msk>(1) | // rising edge
	    bitfield<HRPWM0_HRC_GSEL_C0ES_Msk>(2)| // falling edge
	    bitfield<HRPWM0_HRC_GSEL_S1M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_C1M_Msk>(0) | // use timer
	    bitfield<HRPWM0_HRC_GSEL_S1ES_Msk>(1) | // rising edge
	    bitfield<HRPWM0_HRC_GSEL_C1ES_Msk>(2); // falling edge
	hr->GC|=HRPWM0_HRC_GC_STC_Msk|HRPWM0_HRC_GC_DSTC_Msk
	    | HRPWM0_HRC_GC_DTE_Msk;
	hr->PL=(invert_h? 2:0)|(invert_l? 1:0);

	HIGH{}.enable();
	LOW{}.enable();
    }

    float operator=(float i) {
	float b=ccu8::dev[UNIT].cc[SLICE].PRS*i;
	if constexpr (OUTPUT==0 || OUTPUT==1)
	    ccu8::dev[UNIT].cc[SLICE].CR1S=std::floor(b);
	else
	    ccu8::dev[UNIT].cc[SLICE].CR2S=std::floor(b);
	b-=std::floor(b);
	hrc[SLICE].SCR1=54*b;
	hrc[SLICE].SCR2=54*(1-b);

	return i;
    }
};



}

#endif
