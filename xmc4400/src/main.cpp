#include <atomic>
#include <cmath>

#define M_PI 3.1415927

#include "hardware.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu8.h"
#include "pwm_3phase.h"
#include "udp_logger.h"
#include "udp_poker.h"
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

udp_logger logger __attribute__((section ("ETH_RAM")));
udp_poker poker __attribute__((section ("ETH_RAM")));

extern "C" void SysTick_Handler(void)
{
    // counter++;
}

uint32_t hb[3];
float adc[4];
float adc_scale[4]={0.0010819,0.0010819,1.0,1.0};
int32_t adc_offset[4];

constexpr float servo_factor=0.00185805929607582;
float vservo;

float Istator[2];
float Irotor[2];
float I[2];
float kP[2], kI[2];
float Iset[2];
float Vrotor[2];
float Vstator[2];
float lim=0.85;
float output_scale;
float output[3];
float angle;

enum {
    STARTUP,
    OFFSET_DELAY,
    OFFSET_CALIBRATE,
    ACTIVE,
    MANUAL_ANGLE,
    CURRENT,
    VOLTAGE
} state;

float manual_angle;

extern "C" void CCU80_0_IRQHandler(void)
{
    static_assert(HB0.module==0, "Wrong interrupt handler for HB0");
    static uint32_t counter;

    LED2=0;
    // assert this is the correct handler
    for(int i=0;i<4;i++)
	adc[i]=(int32_t(vadc.G[i].RES[0]&0xffff)-adc_offset[i])*adc_scale[i];
    vservo=dsd.ch[dsd_ch_ns::channel(MDAT)].RESM*servo_factor;

    float current[3];
    current[0]=adc[0];
    current[1]=adc[1];
    current[2]=-current[0]-current[1];

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
	    adc_offset[0]=adc_offset[1]=adc_offset[2]=0;
	}
	break;
    case OFFSET_CALIBRATE:
	if(++counter>=256) {
	    state=CURRENT;
	    for(int i=0;i<4;i++)
		adc_offset[i]/=counter;
	} else {
	    for(int i=0;i<4;i++)
		adc_offset[i]+=vadc.G[i].RES[0]&0xffff;
	}
	break;
    }

    if(!encoder->valid() && !state==MANUAL_ANGLE) {
	switch(state) {
	case ACTIVE:
	    HBEN=1;
	    HB0=hb[0];
	    HB1=hb[1];
	    HB2=hb[2];
	    break;
	default:
	    HBEN=0;
	}
    } else {
	if(state!=MANUAL_ANGLE)
	    angle=encoder->angle();
	else
	    angle=manual_angle;
	Istator[0]=float(3.0/2)*current[0];
	Istator[1]=float(sqrt(3))*current[1]+float(sqrt(3)/2)*current[0];
	Irotor[0]= cosf(angle)*Istator[0]+sinf(angle)*Istator[1];
	Irotor[1]=-sinf(angle)*Istator[0]+cosf(angle)*Istator[1];

	switch(state) {
	case MANUAL_ANGLE:
	case CURRENT:
	    for(int i=0;i<2;i++) {
		float err=Iset[i]-Irotor[i];
		I[i]+=kI[i]*err;
		Vrotor[i]=err*kP[i]+I[i];
	    }
	    // Intentionally no break
	case VOLTAGE:
	    HBEN=1;
	    float sq_len=Vrotor[0]*Vrotor[0]+Vrotor[1]*Vrotor[1];
	    if(sq_len>float(lim*lim)) {
		Vrotor[0]*=lim/sqrtf(sq_len);
		Vrotor[1]*=lim/sqrtf(sq_len);
		I[0]*=lim/sqrtf(sq_len);
		I[1]*=lim/sqrtf(sq_len);
	    }

	    Vstator[0]=cosf(angle)*Vrotor[0]-sinf(angle)*Vrotor[1];
	    Vstator[1]=sinf(angle)*Vrotor[0]+cosf(angle)*Vrotor[1];

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
	    HB0=output_scale*output[0];
	    HB1=output_scale*output[1];
	    HB2=output_scale*output[2];
	    break;
	}
    }

    logger.SetADC(adc[0],adc[1],adc[2],adc[3]);
    logger.SetEncoder(encoder->position(),encoder->angle());
    logger.SetRotor(Vrotor,Irotor);
    logger.SetOutput(output);
    logger.transmit(&eth0);

    static_cast<XMC_CCU8_MODULE_t*>(HB0)->GCSS=0x1111;
    LED2=1;
}

/* This interrupt is used to trigger the encoder */
extern "C" void CCU80_1_IRQHandler(void)
{
    LED3=0;
    static_assert(HB0.module==0, "Wrong interrupt handler for HB0");
    encoder->trigger();
    sleep_counter++;
    LED3=1;
}

/* Receive interrupt channel 1 (full-duplex serial) */
extern "C" void USIC1_0_IRQHandler(void)
{
    // LED3=0;
    encoder->full_duplex();
    // LED3=1;
}

/* Mapped to Frame finished (half-duplex serial) */
extern "C" void USIC0_1_IRQHandler(void)
{
    // static_assert(ENC_TXD.uart_number()==0, "Wrong encoder handler");
    // LED0=0;
    encoder->half_duplex();
    // LED0=1;
}

extern "C" void VADC0_G0_0_IRQHandler(void)
{
    // LED1=0;
    vadc.G[0].REFCLR=vadc.G[0].REFLAG;
    // LED1=1;
}

void init_adc(void);
void init_voltage_measurement(void);
int main()
{
    eth0.add_udp_receiver(&logger,ntohs(1));
    eth0.add_udp_receiver(&poker,ntohs(2));

    // SysTick_Config(SystemCoreClock/1000);
    init_adc();
    pwm_3phase pwm(HB0,HB1,HB2,4*trigger_HZ);
    output_scale=pwm.period();

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
    /*
	Only channel 0 of the ADCs is used. Group 0 is used in queued mode
	only and triggered by the timer at the PWM zero crossing.
	4 results are accumulated in result 1, and put in a fifo to
	be read at result 0.
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
    vadc.GLOBICLASS[0]=vadc_g_ns::iclass_t({{
	.stcs=0,	// no additional cycles
	.cms=0,		// 12-bit conversion
	.stce=0,	// dnc
	.cme=0		// dnc
    }}).raw;

    for(int i=0;i<4;i++) {
	// Channel control
	vadc.G[i].CHCTR[0]=vadc_g_ns::chctr_t({{
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
	// Accumulate 4 results
	vadc.G[i].RCR[1]=vadc_g_ns::rcr_t({{
	    .drctr=3,	// 4 results
	    .dmm=0,	// accumulation 
	    .wfr=0,	// overwrite
	    .fen=0,	// top of fifo
	    .srgen=uint32_t(i==0? 1:0)	// no service request
	}}).raw;
	vadc.G[i].RCR[0]=vadc_g_ns::rcr_t({{
	    .drctr=0,	// 4 results
	    .dmm=0,	// accumulation 
	    .wfr=0,	// overwrite
	    .fen=1,	// part of fifo
	    .srgen=0	// no service request
	}}).raw;
	if(i) {
	    // slave
	    vadc.G[i].SYNCTR=vadc_g_ns::synctr_t({{
		.stsel=1,	// synchronise to G0
		.evalr1=0,
		.evalr2=0,
		.evalr3=0
	    }}).raw;
	} else { 
	    // master
	    // Arbiter, only queued mode is enabled
	    vadc.G[i].ARBPR=vadc_g_ns::arbpr_t({{
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
	    vadc.G[i].QMR0=vadc_g_ns::qmr0_t({{
		.engt=1,
		.entr=1,
		.clrv=0,
		.trev=0,
		.flush=0,
		.cev=0,
		.rptdis=0
	    }}).raw;
	    vadc.G[i].QINR0=vadc_g_ns::qinr0_t({{
		.reqchnr=0,
		.rf=1,
		.ensi=0,
		.extr=1,
		
	    }}).raw;
	    vadc.G[i].QCTRL0=vadc_g_ns::qctrl0_t({{
		.srcresreg=0,	// Use CHCTR.resreg
		.xtsel=8, 	// CCU80::SR2
		.xtlvl=0,
		.xtmode=1,
		.xtwc=1,
		.gtsel=0,
		.gtlvl=0,
		.gtwc=0,
		.tmen=0,	// Uncertain
		.tmwc=1
	    }}).raw;
	    vadc.G[i].SYNCTR=vadc_g_ns::synctr_t({{
		.stsel=0,	// Master
		.evalr1=1,
		.evalr2=1,
		.evalr3=1
	    }}).raw;
	}
    }
    vadc.G[0].ARBCFG=vadc_g_ns::arbcfg_t({{
	.anonc=3,	// permanently on (master/standalone mode)
	.arbrnd=0,	// 4 slots per round
	.arbm=0,	// arbiter runs permanently
	.anons=3	// G0 is the master
    }}).raw;
    NVIC_SetPriority(VADC0_G0_0_IRQn,  0);
    NVIC_ClearPendingIRQ(VADC0_G0_0_IRQn);
    NVIC_EnableIRQ(VADC0_G0_0_IRQn);
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
