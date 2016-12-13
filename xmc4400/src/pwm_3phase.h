#include <array>
#include <xmc_scu.h>
#include <xmc_vadc.h>

class pwm_3phase {
    uint32_t pwm_period;
    int module;
    int internal_slice;
public:
    template <typename A, typename B, typename C>
    pwm_3phase(A &HB0, B& HB1, C &HB2, unsigned frequency);
    uint32_t period(void) { return pwm_period; }
};

template <typename A, typename B, typename C>
pwm_3phase::pwm_3phase(A &HB0, B& HB1, C &HB2, unsigned frequency)
{
    using namespace ccu8_ns;
    static_assert(
	int(HB0.module)==int(HB1.module) && int(HB1.module)==int(HB2.module),
	"All pins on the pwm_3phase object should belong to the same module\n"
    );
    static_assert(
	int(HB0.slice)!=int(HB1.slice) && 
	int(HB1.slice)!=int(HB2.slice) && 
	int(HB2.slice)!=int(HB0.slice),
	"Three different slices are required for pwm_3phase"
    ); 

    module=HB0.module;
    pwm_period=SystemCoreClock/2/frequency;
    {	
	int t=(15 & ~(1<<HB0.slice) & ~(1<<HB1.slice) & ~(1<<HB2.slice));
	internal_slice=0;
	while(!(internal_slice&1))
	    internal_slice++;
    }

    XMC_CCU8_SetModuleClock(HB0, XMC_CCU8_CLOCK_SCU);
    XMC_CCU8_EnableModule(HB0);
    XMC_CCU8_Init(HB0, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

    ccu8[module].GIDLC=GIDLC_t{{.CC0=1, .CC1=1, .CC2=1, .CC3=1, .PRESCALER=1}};
    for(int i=0;i<4;i++) {
	CCU8_CC8_TypeDef &cc=ccu8[module].cc[i];
	cc.INS=INS_t{{
	    .EVENT0_INPUT=7, // GSC8x from SCU_GENERAL.CCUCON
	    .EVENT1_INPUT=0,
	    .EVENT2_INPUT=0,
	    .EVENT0_EDGE=1,
	}};
	cc.CMC=CMC_t{{.EXTERNAL_START=1}};  // use EVENT0 to start
	cc.TCCLR=TCCLR_t{{.TIMER_STOP=1, .TIMER_CLEAR=1}};
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
	if(i==internal_slice) {
	    cc.PRS=4*pwm_period-1;
	    cc.INTE=INTE_t{{
		.PERIOD_MATCH=0,
		.ONE_MATCH=1,		// Encoder transmit
		.COMPARE1_MATCH_UP=1	// Control loop
	    }};
	    cc.SRS=SRS_t{{
		.POSR=1,      	// Match to CC8ySR1 (route to U0C0.DX2F)
		.CM1SR=0	// Control loop ISR
	    }};	
	    cc.CR1S=SystemCoreClock/22000; // Encoder data should have arrived
	} else {
	    if((internal_slice==0 && i==1) || i==0) {
		cc.INTE=INTE_t{{
		    .PERIOD_MATCH=0,
		    .ONE_MATCH=1,		// Trigger ADC
		}};
		cc.SRS=SRS_t{{
		    .POSR=2,      // Match to CC8ySR2 (route to ADC)
		}};	
	    }
	    cc.PRS=pwm_period-1;
	}
    }

    // Start timers and enable interrupt.
    if(!module) {
	uint32_t group=NVIC_GetPriorityGrouping();
	uint32_t encode=NVIC_EncodePriority(group, 63, 0);
	NVIC_SetPriority(CCU80_1_IRQn, encode);
	NVIC_SetPriority(CCU80_0_IRQn, encode);
	NVIC_EnableIRQ(CCU80_1_IRQn);
	NVIC_EnableIRQ(CCU80_0_IRQn);
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC80_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC80_Msk;
    } else {
	NVIC_SetPriority(CCU81_1_IRQn, 
	    NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
	// NVIC_EnableIRQ(CCU80_1_IRQn);
	NVIC_SetPriority(CCU81_0_IRQn, 
	    NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
	NVIC_EnableIRQ(CCU80_0_IRQn);
	SCU_GENERAL->CCUCON|=SCU_GENERAL_CCUCON_GSC81_Msk;
	SCU_GENERAL->CCUCON&=~SCU_GENERAL_CCUCON_GSC81_Msk;
    }

}
