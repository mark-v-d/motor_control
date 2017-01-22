#ifndef PWM_3PHASE_H
#define PWM_3PHASE_H

#include <xmc_scu.h>
#include <xmc_vadc.h>
#include "ccu8.h"
#include "meta.h"
#include "bitfields.h"

class pwm_3phase {
    uint32_t period;
    float kP=0.2;
    float kI=5e-3;
    float integrator, remaining;
public:
    template <class A, class B, class C>
    pwm_3phase(A &HB0, B& HB1, C &HB2, unsigned frequency);
    uint32_t get_period(void) { return period; }
    void adjust(float error);
    void start(void);

    static int constexpr control_irq=0;
    static int constexpr encoder_irq=1;
    static int constexpr adc_irq=2;
    static int constexpr transfer_irq=3;
};

template <class A, class B, class C>
constexpr int spare_slice(A const &a, B const &b, C const &c)
{
    using namespace ccu8_ns;
    static_assert(unit(a)==unit(b) && unit(b)==unit(c),
	"All pins should belong to the same ccu8 unit");
    constexpr int t=(15 & ~(1<<slice(a)) & ~(1<<slice(b)) & ~(1<<slice(c)));
    static_assert(bitcount(t)==1, "Three different slices are required");
    return find_lsb(t);
}

template <typename A, typename B, typename C>
pwm_3phase::pwm_3phase(A &HB0, B &HB1, C &HB2, unsigned frequency)
{
    using namespace ccu8_ns;

    constexpr int module=unit(HB0);

    period=XMC_SCU_CLOCK_GetCcuClockFrequency()/2/frequency;

    XMC_CCU8_SetModuleClock(HB0, XMC_CCU8_CLOCK_SCU);
    XMC_CCU8_EnableModule(HB0);
    XMC_CCU8_Init(HB0, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

    ccu8[module].GIDLC=GIDLC_t{{.CC0=1, .CC1=1, .CC2=1, .CC3=1, .PRESCALER=1}};
    for(int i=0;i<4; i++) {
	auto &cc=ccu8[module].cc[i];
	cc.INS=INS_t{{
	    .EVENT0_INPUT=7, // GSC8x from SCU_GENERAL.CCUCON
	    .EVENT1_INPUT=0,
	    .EVENT2_INPUT=0,
	    .EVENT0_EDGE=1,
	}};
	cc.CMC=CMC_t{{.EXTERNAL_START=1}};  // use EVENT0 to start
	cc.TCCLR=TCCLR_t{{.TIMER_STOP=1, .TIMER_CLEAR=1}};
    }
    for(int i: {slice(HB0), slice(HB1), slice(HB2)}) {
	auto &cc=ccu8[module].cc[i];
	cc.TC=ccu8_cc8_ns::tc_t({{
	    .tcm=1,		// center aligned
	    .tssm=0,
	    .clst=0,
	    .cmod=0,
	    .ecm=0,
	    .capc=0,
	    .tls=0,
	    .endm=0,
	    .strm=1,	// External start also clears the counter
	    .sce=0,		// Equal Capture Event enable
	    .ccs=0,		// Continuous Capture Enable
	    .dithe=1,		// Dither Enable on period
	    .dim=0		// Dither input selector
	}}).raw;
	cc.STC=ccu8_cc8_ns::stc_t({{
	    .cse=0,
	    .stm=0
	}}).raw;
	cc.PRS=period-1;
    }

    {
	auto &cc=ccu8[module].cc[slice(HB0)];
	cc.INTE=INTE_t{{
	    .PERIOD_MATCH=0,
	    .ONE_MATCH=1,		// Trigger ADC
	}};
	cc.SRS=SRS_t{{
	    .POSR=adc_irq,      // Match to CC8ySR2 (route to ADC)
	}};
    }

    {
	auto &cc=ccu8[module].cc[spare_slice(HB0,HB1,HB2)];
	cc.INS=INS_t{{
	    .EVENT0_INPUT=CCU80_IN0_SCU_GSC80, // GSC8x from SCU_GENERAL.CCUCON
	    .EVENT1_INPUT=0,
	    .EVENT2_INPUT=0,
	    .EVENT0_EDGE=1,
	}};
	cc.CMC=CMC_t{{.EXTERNAL_START=1}};  // use EVENT0 to start
	cc.TCCLR=TCCLR_t{{.TIMER_STOP=1, .TIMER_CLEAR=1}};
	cc.TC=TC_t{{
	    .CENTER_ALIGNED=0,
	    .TSSM=0,
	    .CLST=0,
	    .CMOD=0,
	    .ECM=0,
	    .CAPC=0,
	    .TLS=0,
	    .ENDM=0,
	    .STRM=1,		// External start also clears the counter
	    .SCE=0,		// Equal Capture Event enable
	    .CCS=0,		// Continuous Capture Enable
	    .DITHE=0,		// Dither Enable
	    .DIM=0		// Dither input selector
	}};
	cc.PRS=8*period-1;
	cc.INTE=INTE_t{{
	    .PERIOD_MATCH=1,	// Encoder transmit
	    .ONE_MATCH=0,
	    .COMPARE1_MATCH_UP=1, // Control loop
	    .COMPARE1_MATCH_DOWN=0,
	    .COMPARE2_MATCH_UP=1 // transfer
	}};
	cc.SRS=SRS_t{{
	    .POSR=encoder_irq,
	    .CM1SR=control_irq,
	    .CM2SR=transfer_irq
	}};
	cc.CR1S=SystemCoreClock/22000; // Encoder data should have arrived
	cc.CR2S=8*period/2-2*period/3; // Transfer shadow registers
    }
}

inline void pwm_3phase::start(void)
{
    using namespace ccu8_ns;

    // Start timers and enable interrupts.
    NVIC_SetPriority(irq<control_irq>(HB0), 0);
    NVIC_SetPriority(irq<encoder_irq>(HB0), 0);
    NVIC_SetPriority(irq<transfer_irq>(HB0), 0);
    NVIC_EnableIRQ(irq<control_irq>(HB0));
    NVIC_EnableIRQ(irq<encoder_irq>(HB0));
    NVIC_EnableIRQ(irq<transfer_irq>(HB0));
    if(!unit(HB0)){
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC80_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC80_Msk;
    } else {
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC81_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC81_Msk;
    }
}

inline void pwm_3phase::adjust(float error)
{
    using namespace ccu8_ns; 
    integrator+=kI*error;
    remaining+=integrator+kP*error;
    constexpr float limit=10;
    if(remaining>limit) {
	remaining=limit;
	integrator-=kI*error;
    } else if(remaining<-limit){ 
	remaining=-limit;
	integrator-=kI*error;
    }

    int delta=remaining;
    remaining-=delta;

    uint32_t timer=8*period+2*delta-1;
    int32_t timer_dit=delta%4*4;
    uint32_t timer_hr=period+delta/4-1;
    if(timer_dit<0) {
	timer_dit+=16;
	timer_hr-=1;
    }

    for(int i: {slice(HB0), slice(HB1), slice(HB2)}) {
	ccu8[unit(HB0)].cc[i].PRS=timer_hr;
	ccu8[unit(HB0)].cc[i].DITS=timer_dit;
    }
    ccu8[unit(HB0)].cc[spare_slice(HB0,HB1,HB2)].PRS=timer;
}

#endif
