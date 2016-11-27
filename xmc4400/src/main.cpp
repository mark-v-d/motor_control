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

struct position_HC_PQ23_t {
    uint8_t start;
    uint8_t state;
    uint16_t rotation;
    uint16_t encoder;
    uint8_t crc;
    float angle;

    position_HC_PQ23_t(void) {}
    position_HC_PQ23_t(uint8_t *p) {
        start=p[0];
        state=p[1];
        encoder=(p[2]+256*p[3]);
        angle=float(2*M_PI/1024.0)*encoder;
        rotation=p[5]+0x100*p[6];
        crc=p[0];
        for(int i=1;i<9;i++)
	    crc^=p[i];
	// crc should be 0
    }
};

uint8_t rx_buffer[20] __attribute__((section ("ETH_RAM")));
uint8_t *putp;

udp_logger logger __attribute__((section ("ETH_RAM")));
udp_poker poker __attribute__((section ("ETH_RAM")));

extern "C" void SysTick_Handler(void)
{
    counter++;
}

extern "C" void CCU80_0_IRQHandler(void)
{
    LED2=0;
    counter++;
    position_HC_PQ23_t pos(rx_buffer);
    uint32_t adc[4];
    for(int i=0;i<4;i++)
	adc[i]=vadc.G[i].RES[0];

    if(counter>=1500) {
	logger.SetADC(adc[0],adc[1],adc[2],adc[3]);
	logger.SetEncoder(pos.encoder);
	logger.EncoderPacket(rx_buffer);
	logger.transmit(&eth0);
	counter-=1500;
    }

    putp=rx_buffer;
    LED2=1;
}

extern "C" void CCU80_1_IRQHandler(void)
{
    //XMC_UART_CH_Transmit(XMC_UART0_CH0, 0x1a);
    //putp=rx_buffer;
}

extern "C" void USIC1_0_IRQHandler(void)
{
    LED3=0;
    u1c1.PSCR=u1c1.PSR;
    *putp++=u1c1.RBUF;
    LED3=1;
}

extern "C" void VADC0_G0_0_IRQHandler(void)
{
    LED1=0;
    vadc.G[0].REFCLR=vadc.G[0].REFLAG;
    LED1=1;
}


extern ETH_GLOBAL_TypeDef eth_debug;
void uart_init(void);
void init_adc(void);
int main()
{
    SysTick_Config(SystemCoreClock/1000);

    init_adc();
    pwm_3phase(HB0,HB1,HB2,18000);

    eth0.add_udp_receiver(&logger,1);
    eth0.add_udp_receiver(&poker,2);

    counter=0;
    ENC_5V=1;
    ENC_DIR=1;
    uart_init();

    HB1=100;
    PPB->SCR=1;

    XMC_CCU8_EnableShadowTransfer(HB0, 0x1111);

    for(;;);
    int t=0;
    while (1) {
        if(counter>500) {
            //counter-=500;
            LED1^=1;
            HB0=t&1? 2270:2000;
            HB2=t&1? 1500:2000;
            t++;
	    LED3=eth_debug.RX_FRAMES_COUNT_GOOD_BAD&1;
        };
    };

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


void uart_init(void)
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
    XMC_UART_CH_Init(XMC_UART1_CH1, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART0_CH0, 
	XMC_UART_CH_INPUT_RXD, USIC0_C0_DX0_P1_4);
    XMC_UART_CH_SetInputSource(XMC_UART1_CH1, 
	XMC_UART_CH_INPUT_RXD, USIC1_C1_DX0_P0_0);
    XMC_UART_CH_EnableInputInversion(XMC_UART1_CH1,XMC_UART_CH_INPUT_RXD);
    XMC_UART_CH_EnableEvent(XMC_UART1_CH1, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    XMC_UART_CH_SelectInterruptNodePointer(XMC_UART1_CH1, 
	XMC_UART_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);

    if(1) { // timer trigger
	usic_ch_ns::tcsr_t tcsr;
	tcsr.raw=u0c0.TCSR;
	tcsr.tdssm=0;
	tcsr.tdvtr=1;
	u0c0.TCSR=tcsr.raw;

	usic_ch_ns::dx2cr_t dx2cr; dx2cr.raw=0;
	dx2cr.insw=1;
	dx2cr.cm=1;
	dx2cr.dsel=USIC0_C0_DX2_CCU80_SR1;
	u0c0.DXCR[2]=dx2cr.raw;

	u0c0.TBUF[0]=0x1a;
    }

    if(1) {
	// No  DMA
	NVIC_SetPriority(USIC1_0_IRQn,  0);
	NVIC_ClearPendingIRQ(USIC1_0_IRQn);
	NVIC_EnableIRQ(USIC1_0_IRQn);
    } else if(1) {
	// USIC1 SR0 -> DMA0[2]
	XMC_DMA_Enable(XMC_DMA0);
	dma0.CH[2].SAR=reinterpret_cast<uint32_t>(&u1c1.RBUF);
	dma0.CH[2].DAR=reinterpret_cast<uint32_t>(&rx_buffer);
	dma0.CH[2].CTLH=9;
	dma0.CH[2].CTLL=gpdma0_ch0_1_ns::ctll_t({{
	    .int_en=0,
	    .dst_tr_width=XMC_DMA_CH_TRANSFER_WIDTH_8,
	    .src_tr_width=XMC_DMA_CH_TRANSFER_WIDTH_8,
	    .dinc=XMC_DMA_CH_ADDRESS_COUNT_MODE_INCREMENT,
	    .sinc=XMC_DMA_CH_ADDRESS_COUNT_MODE_NO_CHANGE,
	    .dest_msize=XMC_DMA_CH_BURST_LENGTH_1,
	    .src_msize=XMC_DMA_CH_BURST_LENGTH_1,
	    .src_gather_en=0,
	    .dst_scatter_en=0,
	    .tt_fc=XMC_DMA_CH_TRANSFER_FLOW_P2M_DMA,
	    .llp_dst_en=0,
	    .llp_src_en=0
	}}).raw;
	dma0.CH[2].CFGH=gpdma0_ch0_1_ns::cfgh_t({{
	    .fcmode=0,
	    .fifo_mode=0,
	    .protctl=0,
	    .ds_upd_en=0,
	    .ss_upd_en=0,
	    .src_per=4,
	    .dest_per=0
	}}).raw;
	dma0.CH[2].CFGL=gpdma0_ch0_1_ns::cfgl_t({{
	    .ch_prior=0,
	    .ch_susp=0,
	    .fifo_empty=0,
	    .hs_sel_dst=0,
	    .hs_sel_src=0,
	    .lock_ch_l=0,
	    .lock_b_l=0,
	    .lock_ch=0,
	    .lock_b=0,
	    .dst_hs_pol=0,
	    .src_hs_pol=0,
	    .max_abrst=0,
	    .reload_src=0,
	    .reload_dst=0
	}}).raw;
	DLR->SRSEL0=12<<16; // usic1.sr0 mapped to channel 4
	DLR->LNEN=1<<4;
	dma0.CHENREG=0x101<<2;
    } 


    XMC_UART_CH_Start(XMC_UART0_CH0);
    XMC_UART_CH_Start(XMC_UART1_CH1);
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
