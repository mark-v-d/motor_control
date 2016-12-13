#include <atomic>
#include <cmath>

#include "xmc_dma.h"

#define M_PI 3.1415927

#include "gpio.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu8.h"
#include "pwm_3phase.h"
#include "udp_logger.h"
#include "udp_poker.h"
#include "bitfields.h"
#include "encoder.h"



std::atomic<int> counter(0);

#if (UC_DEVICE == XMC4500)
iopin::input<1,14> BUTTON1;
iopin::output<1,0> LED0;
iopin::output<1,1> LED1;
iopin::output<1,2> LED2;
iopin::output<1,3> LED3;
iopin::input<15,8> CLK_RMII;
iopin::input<15,9> CRS_DV;
iopin::input<2,2> RXD0;
iopin::input<2,3> RXD1;
iopin::input<2,4> RXER;
iopin::ETH0_MDC<2,7> MDC;
iopin::ETH0_MDO<2,0> MDIO;
iopin::ETH0_TXD0<2,8> TXD0;
iopin::ETH0_TXD1<2,9> TXD1;
iopin::ETH0_TX_EN<2,5> TX_EN;
#else
iopin::output<0,7> LED0=1;
iopin::output<0,11> LED1=1;
iopin::output<0,4> LED2=1;
iopin::output<0,2> LED3=1;
iopin::output<0,9> LED4=1;
iopin::input<2,1> CLK_RMII;
iopin::input<0,1> CRS_DV;
iopin::input<2,2> RXD0;
iopin::input<2,3> RXD1;
iopin::input<2,4> RXER;
iopin::ETH0_MDC<2,7> MDC;
iopin::ETH0_MDO<2,0> MDIO;
iopin::ETH0_TXD0<2,8> TXD0;
iopin::ETH0_TXD1<2,9> TXD1;
iopin::ETH0_TX_EN<2,5> TX_EN;
iopin::output<1,0> ETH_RESET=1;

iopin::CCU80_OUT20<0,3> HB1;
iopin::CCU80_OUT00<0,5> HB0;
iopin::CCU80_OUT30<0,6> HB2;
iopin::output<0,10> HBEN=1;

iopin::output<0,8> ENC_5V=0;
iopin::output<1,9> ENC_12V=0;
iopin::output<1,15> ENC_DIR=0;
iopin::U0C0_DOUT0<1,5> ENC_TXD; // FIXME, HWCTRL should only be used fo SSI
iopin::input<1,4> ENC_RXD;
#endif

icmpProcessing icmp;
Ethernet eth0(
    0x000319450000ULL, 0, 
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);

uint8_t rx_buffer[20] __attribute__((section ("ETH_RAM")));
volatile int putp;

udp_logger logger __attribute__((section ("ETH_RAM")));
udp_poker poker __attribute__((section ("ETH_RAM")));

extern "C" void SysTick_Handler(void)
{
    counter++;
}

uint32_t hb[3];
float adc[4];
float adc_scale[4]={0.0010819,0.0010819,1.0,1.0};
int32_t adc_offset[4];
int tc;

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

enum {
    STARTUP,
    OFFSET_DELAY,
    OFFSET_CALIBRATE,
    ACTIVE,
    CONTROLLED,
    VOLTAGE
} state;

void init_pos_dummy(position_t*,uint8_t*) {}
void (*init_pos)(position_t*,uint8_t*)=init_pos_dummy;

extern "C" void CCU80_0_IRQHandler(void)
{
    LED2=0;
    position_t pos;
    (*init_pos)(&pos,rx_buffer);
    for(int i=0;i<4;i++)
	adc[i]=(int32_t(vadc.G[i].RES[0]&0xffff)-adc_offset[i])*adc_scale[i];

    float current[3];
    current[0]=adc[0];
    current[1]=adc[1];
    current[2]=-current[0]-current[1];

    float angle=pos.angle;
    Istator[0]=float(3.0/2)*current[0];
    Istator[1]=float(sqrt(3))*current[1]+float(sqrt(3)/2)*current[0];
    Irotor[0]= cosf(angle)*Istator[0]+sinf(angle)*Istator[1];
    Irotor[1]=-sinf(angle)*Istator[0]+cosf(angle)*Istator[1];

    switch(state) {
    case STARTUP:
	HB0=0;
	HB1=0;
	HB2=0;
	state=OFFSET_DELAY;
	break;
    case OFFSET_DELAY:
	if(counter>=1000) {
	    state=OFFSET_CALIBRATE;
	    adc_offset[0]=adc_offset[1]=adc_offset[2]=0;
	    tc=0;
	}
	break;
    case OFFSET_CALIBRATE:
	if(counter>=1000+0x100) {
	    state=ACTIVE;
	    for(int i=0;i<4;i++)
		adc_offset[i]/=tc;
	} else {
	    for(int i=0;i<4;i++)
		adc_offset[i]+=vadc.G[i].RES[0]&0xffff;
	    tc++;
	}
	break;
    case ACTIVE:
	HB0=hb[0];
	HB1=hb[1];
	HB2=hb[2];
	break;
    case CONTROLLED:
	for(int i=0;i<2;i++) {
	    float err=Iset[i]-Irotor[i];
	    I[i]+=kI[i]*err;
	    Vrotor[i]=err*kP[i]+I[i];
	}

    case VOLTAGE:
	float sq_len=Vrotor[0]*Vrotor[0]+Vrotor[1]*Vrotor[1];
	if(sq_len>float(lim*lim)) {
	    Vrotor[0]*=lim/sqrtf(sq_len);
	    Vrotor[1]*=lim/sqrtf(sq_len);
	    I[0]*=lim/sqrtf(sq_len);
	    I[1]*=lim/sqrtf(sq_len);
	}

	Vstator[0]= cosf(angle)*Vrotor[0]-sinf(angle)*Vrotor[1];
	Vstator[1]= sinf(angle)*Vrotor[0]+cosf(angle)*Vrotor[1];

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

    logger.SetADC(adc[0],adc[1],adc[2],adc[3]);
    logger.SetEncoder(pos.encoder,pos.angle);
    logger.SetRotor(Vrotor,Irotor);
    logger.EncoderPacket(rx_buffer);
    logger.SetOutput(output);
    logger.transmit(&eth0);

    counter++;
    static_cast<XMC_CCU8_MODULE_t*>(HB0)->GCSS=0x1111;
    LED2=1;
}

/* This interrupt is used to trigger the encoder */
extern "C" void CCU80_1_IRQHandler(void)
{
    if(init_pos==position_MFS13_13) {
	ENC_DIR=1;
	putp=0;
	u0c0.TBUF[0]=0x12;
    } else if(init_pos==position_HC_PQ23){
	ENC_DIR=1;
	putp=0;
	u0c0.TBUF[0]=0x1a;
    }
}

/* Mapped to Frame finished */
extern "C" void USIC0_1_IRQHandler(void)
{
    LED0=0;
    ENC_DIR=0;
    LED0=1;
}

/* Receive interrupt channel 0 (bi-directional) */
extern "C" void USIC0_0_IRQHandler(void)
{
    LED3=0;
    u0c0.PSCR=u0c0.PSR;
    rx_buffer[putp++]=u0c0.RBUF;
    LED3=1;
}

/* Receive interrupt channel 1 (input only) */
extern "C" void USIC1_0_IRQHandler(void)
{
    LED3=0;
    u1c1.PSCR=u1c1.PSR;
    rx_buffer[putp++]=u1c1.RBUF;
    LED3=1;
}


extern "C" void VADC0_G0_0_IRQHandler(void)
{
    LED1=0;
    vadc.G[0].REFCLR=vadc.G[0].REFLAG;
    LED1=1;
}

extern ETH_GLOBAL_TypeDef eth_debug;
void init_adc(void);
int main()
{
    // SysTick_Config(SystemCoreClock/1000);

    init_adc();
    pwm_3phase pwm(HB0,HB1,HB2,18000);
    output_scale=pwm.period();

    eth0.add_udp_receiver(&logger,1);
    eth0.add_udp_receiver(&poker,2);

    HBEN=0;
    init_mitsubishi();

    HBEN=1;
    PPB->SCR=1;

    XMC_CCU8_EnableShadowTransfer(HB0, 0x1111);

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

void serial_tx(uint8_t data)
{
    ENC_DIR=1;
    putp=0;
    u0c0.TBUF[0]=0x1a;
    uint32_t old_counter=counter;
    while(counter<old_counter+1)
	;
}

/*
    Configure the Mitsubishi encoder interface
    returns 1 if failed.
*/
int init_mitsubishi(void)
{
    XMC_UART_CH_CONFIG_t uart_config = {
	.baudrate=2500000,
	.data_bits=8U,
	.frame_length=0,
	.stop_bits=1,
	.oversampling=0,
	.parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
    };
    XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART0_CH0, 
	XMC_UART_CH_INPUT_RXD, USIC0_C0_DX0_P1_4);

    // Configure to send single characters
    u0c0.TCSR=usic_ch_ns::tcsr_t({{
	.wlemd=0,
	.selmd=0,
	.flemd=0,
	.wamd=0,
	.hpcmd=0,
	.sof=0,
	.eof=0,
	.tdv=0,		// read-only
	.tdssm=1,	// single shot-mode 
	.tden=1,
	.tdvtr=0
    }}).raw;

    // Receive channel 0
    NVIC_SetPriority(USIC0_0_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC0_0_IRQn);
    NVIC_EnableIRQ(USIC0_0_IRQn);

    // End transmittting
    NVIC_SetPriority(USIC0_1_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC0_1_IRQn);
    NVIC_EnableIRQ(USIC0_1_IRQn);

    // XMC_UART_CH_Start(XMC_UART0_CH0);
    u0c0.INPR=usic_ch_ns::inpr_t({{
	.tsinp=0,	// End transmission (not used)
	.tbinp=0,	// Start transmission (not used)
	.rinp=0,	// Receive interrupt
	.ainp=0,	// alternate receive interrupt (not used)
	.pinp=1		// protocol interrupt (frame finished)
    }}).raw;
    u0c0.CCR=usic_ch_ns::ccr_t({{
	.mode=2,	// UART
	.hpcen=0,
	.pm=0,		// No parity
	.rsien=0,
	.dlien=0,
	.tsien=0,	// shifter finished 
	.tbien=0,	// transmit buffer
	.rien=1,	// receive interrupt
	.aien=0,
	.brgien=0
    }}).raw;
    u0c0.PCR=usic_ch_ns::pcr_asc_t({{
	.smd=1,
	.stpb=0,
	.idm=0,
	.sbien=0,
	.cden=0,
	.rnien=0,
	.feien=0,
	.ffien=1,	// frame finished enable
	.sp=9,
	.pl=0,
	.rsten=0,
	.tsten=0,
	.mclk=0
    }}).raw;

    // Powerup and wait
    serial_tx(0x1a);
    ENC_5V=1;
    uint32_t old_counter=counter;
    while(counter<old_counter+450)
	;

    // First try to use the bi-directional mode
    serial_tx(0x1a);
    if(putp==9) {
	init_pos=position_MFS13_13;
	return 0;
    }

    XMC_UART_CH_Init(XMC_UART1_CH1, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART1_CH1, 
	XMC_UART_CH_INPUT_RXD, USIC1_C1_DX0_P0_0);
    XMC_UART_CH_EnableInputInversion(XMC_UART1_CH1,XMC_UART_CH_INPUT_RXD);
    XMC_UART_CH_EnableEvent(XMC_UART1_CH1, 
	XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    XMC_UART_CH_SelectInterruptNodePointer(XMC_UART1_CH1, 
	XMC_UART_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);
    XMC_UART_CH_Start(XMC_UART1_CH1);
    // Receive on channel 1
    NVIC_SetPriority(USIC1_0_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC1_0_IRQn);
    NVIC_EnableIRQ(USIC1_0_IRQn);


    serial_tx(0x1a);
    if(putp!=9) {
	ENC_5V=0;
	ENC_DIR=0;
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC0);
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	return 1; 
    }

    init_pos=position_HC_PQ23;

    return 0;
}
