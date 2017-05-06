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
#include "pwm_3phase.h"
#include "udp_logger.h"
#include "udp_poker.h"
#include "udp_sync.h"
#include "bitfields.h"
#include "encoder.h"
#include <arpa/inet.h>

std::atomic<uint32_t> sleep_counter(0);

icmpProcessing icmp;
Ethernet eth0(
    0, 
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);

udp_logger::input_t in;
udp_logger::output_t out;

udp_logger logger __attribute__((section ("ETH_RAM"))) (&in);
udp_poker poker __attribute__((section ("ETH_RAM")));
udp_sync syncer __attribute__((section ("ETH_RAM")));
pwm_3phase <decltype(HB0),decltype(HB1),decltype(HB2)>pwm(4*trigger_HZ);

extern "C" void SysTick_Handler(void)
{
    // counter++;
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
    VOLTAGE
} state;

float manual_angle;

struct {
    volatile uint32_t timer[2];
    volatile uint32_t capture[2];
} posif_data;

extern "C" void CCU80_0_IRQHandler(void)
{
    static_assert(ccu8_ns::unit(HB0)==0, "Wrong interrupt handler for HB0");
    static_assert(pwm.control_irq==0, "Wrong handler");
    static uint32_t counter;
    uint32_t output_scale=pwm.get_period();

    LED2=0;
    for(int i=0;i<4;i++)
	out.adc[i]=(int32_t(vadc.G[i].RES[0]&0xffff)-adc_offset[i])*adc_scale[i];
    out.vservo=dsd.ch[dsd_ch_ns::channel(MDAT)].RESM*servo_factor;

    out.counter= uint32_t(ccu40.cc[0].CV[1])<<16 | ccu40.cc[0].TIMER;

    posif_data.timer[0]=(ccu40.cc[1].TIMER<<16) | ccu40.cc[0].TIMER;
    posif_data.timer[1]=(ccu40.cc[3].TIMER<<16) | ccu40.cc[2].TIMER;
    posif_data.capture[0]=(ccu40.cc[1].CV[1]<<16) | ccu40.cc[0].CV[1];
    posif_data.capture[1]=(ccu40.cc[3].CV[1]<<16) | ccu40.cc[2].CV[1];

    float current[3];
    current[0]=out.adc[0];
    current[1]=out.adc[3];
    current[2]=-current[0]-current[1];

    int valid=encoder->valid();

    if(!valid) {
	out.angle=1000;
	LED1=0;
    } else {
	LED1=1;
	out.position=encoder->position();
    }

    switch(state) {
    case STARTUP:
	HBEN=0;
	HB0=0;
	HB1=0;
	HB2=0;
	state=OFFSET_DELAY;
	counter=0;
	break;
    case OFFSET_DELAY:
	if(++counter>=1000) {
	    state=OFFSET_CALIBRATE;
	    counter=0;
	    adc_offset[0]=adc_offset[3]=0;
	}
	break;
    case OFFSET_CALIBRATE:
	if(++counter>=4096) {
	    state=CURRENT;
	    for(int i:{0,3})
		adc_offset[i]/=counter;
	} else {
	    for(int i:{0,3})
		adc_offset[i]+=vadc.G[i].RES[0]&0xffff;
	}
	break;
    }

    if(valid) {
	float angle=encoder->angle();
	if(state==MANUAL_ANGLE || state==MANUAL_VOLTAGE)
	    angle=manual_angle;
	float Istator[2];
	Istator[0]=float(3.0/2)*current[0];
	Istator[1]=float(sqrt(3))*current[1]+float(sqrt(3)/2)*current[0];
	out.Irotor[0]= cosf(angle)*Istator[0]+sinf(angle)*Istator[1];
	out.Irotor[1]=-sinf(angle)*Istator[0]+cosf(angle)*Istator[1];
	out.angle=angle;

	switch(state) {
	case MANUAL_ANGLE:
	case CURRENT:
	    for(int i=0;i<2;i++) {
		float err=in.Iset[i]-out.Irotor[i];
		out.I[i]+=in.kI[i]*err;
		out.Vrotor[i]=err*in.kP[i]+out.I[i];
	    }
	    // Intentionally no break
	case VOLTAGE:
	case MANUAL_VOLTAGE:
	    HBEN=1;
	    float sq_len=out.Vrotor[0]*out.Vrotor[0]
		+out.Vrotor[1]*out.Vrotor[1];
	    if(sq_len>float(in.lim*in.lim)) {
		out.Vrotor[0]*=in.lim/sqrtf(sq_len);
		out.Vrotor[1]*=in.lim/sqrtf(sq_len);
		out.I[0]*=in.lim/sqrtf(sq_len);
		out.I[1]*=in.lim/sqrtf(sq_len);
	    }

	    float Vstator[2];
	    Vstator[0]=cosf(angle)*out.Vrotor[0]-sinf(angle)*out.Vrotor[1];
	    Vstator[1]=sinf(angle)*out.Vrotor[0]+cosf(angle)*out.Vrotor[1];

	    float output[3];
	    if(	(output[0]=float(sqrt(3)/3)*Vstator[1]+Vstator[0])>=0 &&
		(output[1]=float(2/sqrt(3))*Vstator[1])>=0
	    ) {
		output[2]=0;
	    } else if(
		(output[1]=-Vstator[0]+float(sqrt(3)/3)*Vstator[1])>=0 &&
		(output[2]=-float(sqrt(3)/3)*Vstator[1]-Vstator[0])>=0
	    ) {
		output[0]=0;
	    } else {
		output[0]=Vstator[0]-float(sqrt(3)/3)*Vstator[1];
		output[1]=0;
		output[2]=-float(2/sqrt(3))*Vstator[1];
	    }
	    for(int i=0;i<3;i++)
		out.output[i]=output[i];
	    HB0=output_scale*(1.0001F-output[0]);
	    HB1=output_scale*(1.0001F-output[1]);
	    HB2=output_scale*(1.0001F-output[2]);
	    break;
	}
    }

    logger.transmit(&eth0,out);

    LED2=1;
}

/* This interrupt is used to trigger the encoder */
extern "C" void CCU80_1_IRQHandler(void)
{
    LED3=0;
    static_assert(ccu8_ns::unit(HB0)==0, "Wrong interrupt handler for HB0");
    static_assert(pwm.encoder_irq==1, "Wrong handler");
    encoder->trigger();
    sleep_counter++;
    LED3=1;
}

/* This interrupt is used to transfer the data */
extern "C" void CCU80_3_IRQHandler(void)
{
    LED3=0;
    static_assert(ccu8_ns::unit(HB0)==0, "Wrong interrupt handler for HB0");
    static_assert(pwm.transfer_irq==3, "Wrong handler");
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
extern "C" void USIC0_1_IRQHandler(void)
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

void init_adc(void);
void init_voltage_measurement(void);
int main()
{
    eth0.add_udp_receiver(&logger,ntohs(1));
    eth0.add_udp_receiver(&poker,ntohs(2));
    eth0.add_udp_receiver(&syncer,ntohs(3));
    pwm.start();

    // SysTick_Config(SystemCoreClock/1000);
    init_adc();

    PPB->SCR=1;

    XMC_CCU8_EnableShadowTransfer(HB0, 0x1111);

    init_voltage_measurement();
    init_encoder();

    for(;;)
	;
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
	.divwc=1,
	.dp_cal0=0,
	.dp_cal1=0,
	.dp_cal2=0,
	.dp_cal3=0,
	.sucal=0
    }}).raw;
    vadc.GLOBICLASS[0]=iclass_t({{
	.stcs=0,	// no additional cycles
	.cms=0,		// 12-bit conversion
	.stce=0,	// dnc
	.cme=0		// dnc
    }}).raw;

    vadc.G[1].ALIAS=alias_t({{.alias0=6}}).raw;
    vadc.G[3].ALIAS=alias_t({{.alias0=2}}).raw;

    for(int i=0;i<4;i++) { // All channels
	// Channel control
	vadc.G[i].CHCTR[0]=chctr_t({{
	    .iclsel=2,		// global class 0
	    .bndsell=0,		// dnc
	    .bndselu=0,		// dnc
	    .chevmode=0,	// no event
	    .sync=1,		// Synchronised conversion (G0 master)
	    .refsel=0,
	    .resreg=1,		// Top of 2 entry fifo
	    .restbs=0,
	    .respos=0,
	    .bwdch=0,
	    .bwden=0
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
	    .evalr1=0,
	    .evalr2=0,
	    .evalr3=0
	}}).raw;
    }
    { 
	// master
	int i=0;
	// Arbiter, only queued mode is enabled
	vadc.G[i].ARBPR=arbpr_t({{
	    .prio0=3,
	    .csm0=1,
	    .prio1=0,
	    .csm1=0,
	    .prio2=0,
	    .csm2=0,
	    .asen0=1,
	    .asen1=0,
	    .asen2=0
	}}).raw;
	vadc.G[i].QMR0=qmr0_t({{
	    .engt=1,
	    .entr=1,
	    .clrv=0,
	    .trev=0,
	    .flush=0,
	    .cev=0,
	    .rptdis=0
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
	    .srcresreg=0,	// Use CHCTR.resreg
	    .xtsel=8, 	// CCU80::SR2 (See asserts)
	    .xtlvl=0,
	    .xtmode=1,
	    .xtwc=1,
	    .gtsel=0,
	    .gtlvl=0,
	    .gtwc=0,
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

#include "xmc_dsd.h"

void init_voltage_measurement(void)
{
    using namespace dsd_ch_ns;

    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_DSD);
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_DSD);
    dsd.CLC=0;
    dsd.GLOBCFG=1;

    constexpr int ch=channel(MDAT);
    dsd.ch[ch].MODCFG=modcfg_t({{
	.divm=15,
	.dwc=1
    }}).raw;
    dsd.ch[ch].DICFG=dicfg_t({{
	.dsrc=not_dsrc(MDAT), // data inverted because input is swapped
	.dswc=1,	// Enable dsrc update
	.itrmode=0,	// integrator bypassed
	.tstrmode=0,	// No timestamp trigger
	.trsel=0,	// Don't care, trigger not used
	.trwc=1,	// Write Control for Trigger Parameters
	.csrc=15, 	// Internal clock
	.strobe=1,	// sample trigger is generated at each rising clock edge
	.scwc=1		// write control
    }}).raw;
    dsd.ch[ch].FCFGC=fcfgc_t({{
	.cfmdf=63,	// decimate by 256
	.cfmc=2,	// 3rd order
	.cfen=1,	// enable CIC
	.srgm=0,	// no service requests
	.cfmsv=3,	// start value ?!?
	.cfmdcnt=10 	// Decimation counter
    }}).raw;
    dsd.GLOBRC|=1<<ch; 
}
