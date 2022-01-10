/*
    PQ23,
	30.5mH, 7.5 Ohm @ 400Hz u->v, w open
	23.0mH, 5.2 Ohm @ 400Hz u->v+w
*/

#include <atomic>
#include <cmath>

#include <math.h>
constexpr auto PI=acos(-1);

#include "hardware.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu8.h"
#include "udp_logger.h"
#include "udp_poker.h"
#include "udp_sync.h"
#include "bitfields.h"
#include "encoder.h"
#include <arpa/inet.h>

#include "bsl.h"
// using namespace std::complex_literals;
using namespace std::chrono_literals;

ccu8_2::half_bridge test_out(HBH0,HBL0);


auto copro=uart::make_full_duplex_no_int(COPRO_TXD,COPRO_RXD);

std::atomic<uint32_t> sleep_counter(0);

icmpProcessing icmp;
/*
Ethernet eth0(
    0,
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);
*/

udp_logger::input_t in;
udp_logger::output_t out;

udp_logger logger __attribute__((section ("ETH_RAM"))) (&in);
udp_poker poker __attribute__((section ("ETH_RAM")));
udp_sync syncer __attribute__((section ("ETH_RAM")));

extern "C" void SysTick_Handler(void)
{
    static uint8_t counter;
    ITM->PORT[1].u8=counter++;
}

uint32_t hb[3];
float adc[4];
float adc_scale[4]={0.0012099,1.0,1.0,0.0012436};
int32_t adc_offset[4]={0,2047,2047,0};

constexpr float servo_factor=0.00185805929607582;

enum {
    STARTUP,
    OFFSET_DELAY,
    OFFSET_CALIBRATE,
    ACTIVE,
    MANUAL_ANGLE,
    MANUAL_VOLTAGE,
    CURRENT,
    VOLTAGE,
    OVERRIDE
} state;

float manual_angle;
float angle_offset=0;

extern "C" void CCU80_0_IRQHandler(void)
{
    test_out=out.output[0];
    ccu8_2::shadow_transfer(test_out);
}

/* This interrupt is used to trigger the encoder */
extern "C" void CCU80_1_IRQHandler(void)
{
    LED3=0;
    static_assert(ccu8_ns::unit(HB0)==0, "Wrong interrupt handler for HB0");
    encoder->trigger();
    copro.tx(sleep_counter&255);
    sleep_counter++;
    ITM->PORT[9].u32=sleep_counter;
    LED3=1;
}

/* This interrupt is used to transfer the data */
extern "C" void CCU80_3_IRQHandler(void)
{
    LED3=0;
    static_assert(ccu8_ns::unit(HB0)==0, "Wrong interrupt handler for HB0");
    constexpr uint32_t shadow_transfer=0x1111
	| (2<<4*ccu8_ns::slice(HB0))
	| (2<<4*ccu8_ns::slice(HB1))
	| (2<<4*ccu8_ns::slice(HB2));
    ccu8[ccu8_ns::unit(HB0)].GCSS=shadow_transfer;
    LED3=1;
}

/* Receive interrupt (full-duplex serial) */
extern "C" void USIC0_0_IRQHandler(void)
{
    LED3=0;
    static_assert(encoder_t::fd_irq==0, "Full duplex should be mapped to IRQ0");
    static_assert(usic_ch_ns::unit(ENC_RXD)==0, "Invalid unit mapping");
    encoder->full_duplex();
    LED3=1;
}

/* Mapped to Frame finished (half-duplex serial) */
extern "C"
__attribute__((section(".ram_code")))
void USIC0_1_IRQHandler(void)
{
    LED3=0;
    static_assert(encoder_t::hd_irq==1, "Half duplex should be mapped to IRQ1");
    static_assert(usic_ch_ns::unit(ENC_TXD)==0, "Invalid unit mapping");
    encoder->half_duplex();
    LED3=1;
}

extern "C" void VADC0_G0_0_IRQHandler(void)
{
    LED3=0;
    vadc.G[0].REFCLR=vadc.G[0].REFLAG;
    LED3=1;
}

volatile uint32_t counter, led, txd=-1;

void init_adc(void);
int main()
{
    /*
    eth0.add_udp_receiver(&logger,ntohs(1));
    eth0.add_udp_receiver(&poker,ntohs(2));
    eth0.add_udp_receiver(&syncer,ntohs(3));
    */
    ETH_RESET=1; ETH_RESET.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_5V=0; ENC_5V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_12V=0; ENC_12V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_DIR=0; ENC_DIR.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO7=0; IO7.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);	// power enable copro

    // Turn traceport on.
    LED0.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED0.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED1.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED1.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED2.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED2.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED3.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED3.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED4.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED4.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);

    SysTick_Config(1000000);
    SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);
    tpi;

    //init_adc();

    PPB->SCR=1;

    //init_encoder();
    bsl_init(IO7,COPRO_TXD,COPRO_RXD);
    copro.SetBaudrate(1e6);

    ccu8_2::init<test_out.UNIT>(
	XMC_CCU8_CLOCK_SCU,
	XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR
    );
    test_out.init();
    test_out.period(1s/20000.0f);
    test_out.deadtime(1us);
    test_out=0.25f;

    test_out->INTE=CCU8_CC8_INTE_PME_Msk;
    test_out->SRS=bitfield<CCU8_CC8_SRS_POSR_Msk>(0);
    NVIC_SetPriority(test_out.irq<0>(), 0);
    NVIC_EnableIRQ(test_out.irq<0>());

    ccu8_2::shadow_transfer(test_out);
    ccu8_2::start(test_out);

    out.output[0]=0.1f;

    auto old_led=led;
    for(;;) {
	if(txd!=-1) {
	    copro.tx(txd);
	    copro.tx(txd);
	    led^=1;
	    txd=-1;
	}
	if(led!=old_led) {
	    old_led=led;
	    LED0=old_led&1;
	    LED1=(old_led>>1)&1;
	    LED2=(old_led>>2)&1;
	    LED3=(old_led>>3)&1;
	    counter++;
	}
    }
    return 0;
}


/* This removes a lot of code from the binary
*/
void __gnu_cxx::__verbose_terminate_handler(void)
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED2.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED3.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    for(;;) {
	LED0^=1;
	LED1^=1;
	LED2^=1;
    }
}

void init_adc(void)
{
    using namespace vadc_g_ns;
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
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_VADC);
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_VADC);
    vadc.CLC=0;
    vadc.GLOBCFG=vadc_ns::globcfg_t({{
	.diva=3,
	.dcmsb=0,
	.divd=1,
	.divwc=1
    }}).raw;
    vadc.GLOBICLASS[0]=iclass_t({{
	.stcs=0,	// no additional cycles
	.cms=0		// 12-bit conversion
    }}).raw;

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
}
