#include <xmc_scu.h>
#include <xmc_vadc.h>
#include "ccu8.h"
#include "meta.h"

class pwm_3phase {
    uint32_t pwm_period;
public:
    template <typename A, typename B, typename C>
    pwm_3phase(A &HB0, B& HB1, C &HB2, unsigned frequency);
    uint32_t period(void) { return pwm_period; }

    static int constexpr control_irq=0;
    static int constexpr encoder_irq=1;
    static int constexpr adc_irq=2;
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

    pwm_period=XMC_SCU_CLOCK_GetCcuClockFrequency()/2/frequency;

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
	cc.TC=TC_t{{
	    .CENTER_ALIGNED=1,
	    .TSSM=0,
	    .CLST=0,
	    .CMOD=0,
	    .ECM=0,
	    .CAPC=0,
	    .TLS=0,
	    .ENDM=0,
	    .STRM=1	// External start also clears the counter
	}};
	cc.PRS=pwm_period-1;
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
	cc.PRS=8*pwm_period-1;
	cc.INTE=INTE_t{{
	    .PERIOD_MATCH=1,	// Encoder transmit
	    .ONE_MATCH=0,
	    .COMPARE1_MATCH_UP=1 // Control loop
	}};
	cc.SRS=SRS_t{{
	    .POSR=encoder_irq,
	    .CM1SR=control_irq
	}};
	cc.CR1S=SystemCoreClock/22000; // Encoder data should have arrived
    }

    // Start timers and enable interrupt.
    NVIC_SetPriority(irq<1>(HB0), 0);
    NVIC_SetPriority(irq<0>(HB0), 0);
    NVIC_EnableIRQ(irq<1>(HB0));
    NVIC_EnableIRQ(irq<0>(HB0));
    if(!module){
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC80_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC80_Msk;
    } else {
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC81_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC81_Msk;
    }
}
