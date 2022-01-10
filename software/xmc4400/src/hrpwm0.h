#ifndef HRPWM0_H
#define HRPWM0_H

#include <xmc_hrpwm.h>

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
    static constexpr int UNIT=0;

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

inline auto init(void)
{
     auto status=XMC_HRPWM_Init(&dev);
     XMC_HRPWM_EnableGlobalHR(&dev);
     return XMC_HRPWM_GetHRGenReadyStatus(&dev);
     //return status;
}

template <int UNIT_PAR>
class slice_t {
public:
    static constexpr int UNIT=UNIT_PAR;

    HRPWM0_HRC_Type* operator->(void) { return &hrc[UNIT]; }

    void low_resolution(void) {
	dev.HRCCFG&=~(HRPWM0_HRCCFG_HRC0E_Msk<<UNIT);
	dev.HRCCFG|=(HRPWM0_HRCCFG_LRC1E_Msk|HRPWM0_HRCCFG_LRC0E_Msk)<<UNIT;
    }
    void shadow_transfer(void) {
	dev.HRCSTRG=(HRPWM0_HRCSTRG_H0ES_Msk|HRPWM0_HRCSTRG_H0DES_Msk)<<(4*UNIT);
    }
};

}

#endif
