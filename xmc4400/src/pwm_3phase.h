#include <array>

class pwm_3phase {
    uint32_t pwm_period;
    int module;
    std::array<int,4> slice;
public:
    template <typename A, typename B, typename C>
    pwm_3phase(A &HB0, B& HB1, C &HB2, unsigned frequency);
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
    slice[0]=HB0.slice;
    slice[1]=HB1.slice;
    slice[2]=HB2.slice;

    // slice[3] is used for low frequency triggering
    switch(15 & ~(1<<HB0.slice) & ~(1<<HB1.slice) & ~(1<<HB2.slice)) {
    case 1: slice[3]=0; break;
    case 2: slice[3]=1; break;
    case 4: slice[3]=2; break;
    case 8: slice[3]=3; break;
    }

    ccu8[module].GCTRL=GCTRL_t{{.PRBC=0, .PCIS=0}};

    // XMC_CCU8_SetModuleClock(HB0, XMC_CCU8_CLOCK_SCU);
    XMC_CCU8_EnableModule(HB0);
    XMC_CCU8_Init(HB0, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

    ccu8[module].GIDLC=GIDLC_t{{.CC0=1, .CC1=1, .CC2=1, .CC3=1, .PRESCALER=1}};

    pwm_period=SystemCoreClock/2/frequency;
    for(int i=0;i<4;i++) {
	CCU8_CC8_TypeDef &cc=ccu8[module].cc[slice[i]];
	cc.TCCLR=TCCLR_t{.TRBC=1};
	cc.TC=TC_t{.CENTER_ALIGNED=1};
	// The interrupt subsamples by 4
	cc.PRS=i==3? 4*pwm_period+3:pwm_period;
	cc.TCSET=TCCLR_t{.TRBC=1};
    }

    NVIC_SetPriority(CCU80_1_IRQn, 
	NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_EnableIRQ(CCU80_1_IRQn);

    CCU8_CC8_TypeDef &cc=ccu8[module].cc[slice[3]];
    cc.INTE=INTE_t{.ONE_MATCH=1};
    cc.SRS=SRS_t{.POSR=1};	// Match to CC8ySR1 (route to U0C0.DX2F)
#if 0

    XMC_CCU8_SLICE_StartTimer(HB0);
    XMC_CCU8_SLICE_StartTimer(HB1);
    XMC_CCU8_SLICE_StartTimer(HB2);

    XMC_CCU8_StartPrescaler(HB0);
    XMC_CCU8_EnableClock(HB0, HB0);
    XMC_CCU8_EnableClock(HB0, HB1);
    XMC_CCU8_EnableClock(HB0, HB2);

    XMC_CCU8_SLICE_StartTimer(HB0);
    XMC_CCU8_SLICE_StartTimer(HB1);
    XMC_CCU8_SLICE_StartTimer(HB2);
    XMC_CCU8_SLICE_SetEvent(HB0, 
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1);
    XMC_CCU8_SLICE_EnableEvent(HB0,
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1);
    /* Connect compare match event to SR1 */
    XMC_CCU8_SLICE_SetInterruptNode(HB0, 
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1, XMC_CCU8_SLICE_SR_ID_1);

    /* Enable IRQ */
#endif
}
