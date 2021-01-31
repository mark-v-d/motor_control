#include "gpio.h"
#include "uart.h"

gpio::output<0,0> DAC;
gpio::pin<0,14> RXD;
gpio::pin<0,15> TXD;

auto serial=uart::make_full_duplex_no_int(TXD,RXD);

extern "C" void SysTick_Handler(void)
{
    static uint8_t i;
	serial.tx(i++);
    DAC.toggle();
}

namespace adc {
void init(void);
}

int main(int argc, char **argv)
{
    DAC.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    SysTick_Config(1000000);
    //NVIC_SetPriorityGrouping(0);
    //NVIC_SetPriority(SysTick_IRQn,0);
    NVIC_EnableIRQ(SysTick_IRQn);

    serial.init(57600);

    adc::init();

    for(;;) {
    }
    return 0;
}

namespace adc {
#include "xmc_vadc.h"

#if UC_FAMILY == XMC1
static constexpr int group_count=2;
#else
static constexpr int group_count=4;
#endif

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

void init(void)
{
    #if defined(PERIPHERAL_RESET_SUPPORTED)
	XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_VADC);
    #endif
    XMC_VADC_GLOBAL_EnableModuleClock(&vadc);
    XMC_VADC_GLOBAL_StartupCalibration(&vadc);

    // Don't use compatible timing mode
    vadc.GLOBCFG=VADC_GLOBCFG_DIVWC_Msk
	| bitfield<VADC_GLOBCFG_DIVA_Msk>(0)
	| bitfield<VADC_GLOBCFG_DIVD_Msk>(0);

    // Only using global class 0
    vadc.GLOBICLASS[0]=
	bitfield<VADC_GLOBICLASS_CMS_Msk>(0)// 12-bit conversion
	| bitfield<VADC_GLOBICLASS_STCS_Msk>(0); // Sample time

    // Reference voltage?


#if 0
    /*
	FIXME, no checks or automation.

	Only channel 0 of the ADCs is used. Group 0 is used in queued mode
	only and triggered by the timer at the PWM zero crossing.
	4 results are accumulated in result 1, and put in a fifo to
	be read at result 0.

	   G0CH0	master CUR0
	   G1CH6	ENC_SIN (alias)
	   G2CH0	ENC_COS
	   G3CH2	CUR1 (alias)
    */

    vadc.G[1].ALIAS=alias_t({{.alias0=6}}).raw;
    vadc.G[3].ALIAS=alias_t({{.alias0=2}}).raw;

    for(int i=0;i<4;i++) { // All channels
	// Channel control
	vadc.G[i].CHCTR[0]=chctr_t({{
	    .iclsel=2,		// global class 0
	    .chevmode=0,	// no event
	    .sync=1,		// Synchronised conversion (G0 master)
	    .resreg=1		// Top of 2 entry fifo
	}}).raw;
	vadc.G[i].RCR[0]=rcr_t({{
	    .drctr=0,	// 4 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=1,	// part of fifo
	    .srgen=0	// no service request
	}}).raw;
    }
    for(int i: {0,3}) {	// Current measurement channels
	// Current measurement is averaged
	vadc.G[i].RCR[1]=rcr_t({{
	    .drctr=3,	// 4 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=0,	// top of fifo
	    .srgen=uint32_t(i==0? 1:0)	// no service request (only master)
	}}).raw;
    }
    for(int i: {1,2}){
	vadc.G[i].RCR[1]=rcr_t({{ // Encoder channels
	    // sincos is not averaged
	    .drctr=0,// 1 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=0,	// top of fifo
	    .srgen=0	// no service request
	}}).raw;
    }
    for(int i=1;i<4;i++) {
	// slave
	vadc.G[i].SYNCTR=synctr_t({{
	    .stsel=1,	// synchronise to G0
	}}).raw;
    }
    {
	// master
	int i=0;
	// Arbiter, only queued mode is enabled
	vadc.G[i].ARBPR=arbpr_t({{
	    .prio0=3,
	    .csm0=1,
	    .asen0=1,
	}}).raw;
	vadc.G[i].QMR0=qmr0_t({{
	    .engt=1,
	    .entr=1
	}}).raw;
	vadc.G[i].QINR0=qinr0_t({{
	    .reqchnr=0,
	    .rf=1,
	    .ensi=0,
	    .extr=1,

	}}).raw;
	// FIXME, make the xtsel mapping automatic
	static_assert(ccu8_ns::unit(HB0)==0, "Wrong timer for ADC trigger");
	static_assert(pwm.adc_irq==2, "Wrong ADC trigger");
	vadc.G[i].QCTRL0=qctrl0_t({{
	    .xtsel=8, 	// CCU80::SR2 (See asserts)
	    .xtmode=1,
	    .xtwc=1,
	    .tmen=0,	// Uncertain
	    .tmwc=1
	}}).raw;
	vadc.G[i].SYNCTR=synctr_t({{
	    .stsel=0,	// Master
	    .evalr1=1,
	    .evalr2=1,
	    .evalr3=1
	}}).raw;
    }
    vadc.G[0].ARBCFG=arbcfg_t({{
	.anonc=3,	// permanently on (master/standalone mode)
	.arbrnd=0,	// 4 slots per round
	.arbm=0,	// arbiter runs permanently
	.anons=3	// G0 is the master
    }}).raw;
#if 0
    NVIC_SetPriority(VADC0_G0_0_IRQn,  0);
    NVIC_ClearPendingIRQ(VADC0_G0_0_IRQn);
    NVIC_EnableIRQ(VADC0_G0_0_IRQn);
#endif
#endif
}

}
