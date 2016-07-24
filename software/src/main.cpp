#include <math.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "misc.h"
#include "gpio_init.h"
#include "encoder.h"

#define PRESCALE 2
#define FULL_SCALE 0xde00

float output[3];
float current[3];
float Istator[2];
float Irotor[2];
float Vout[2];
position_t position;

float buffer[1000];
volatile int putp;

extern "C" void HRTIM1_TIMA_IRQHandler(void)
{
    LED2=0;
    HRTIM1->HRTIM_TIMERx[0].TIMxICR=HRTIM_TIMICR_REPC;
    HRTIM1->HRTIM_TIMERx[1].SETx2R=HRTIM_SET2R_SST;
    LED2=1;
}
extern "C" void HRTIM1_TIMB_IRQHandler(void)
{
    static int counter=0;

    LED2=0;
    HRTIM1->HRTIM_TIMERx[1].TIMxICR=HRTIM_TIMICR_RST2C;
    position=rx_buffer;

    for(int i=0;i<3;i++)
	output[i]=max(0.0F,min(0.95F*FULL_SCALE,output[i]));

    current[0]=0.0013966F*(ADC1->JDR1-2082.6F);
    current[1]=0.0013024F*(ADC2->JDR1-2059.9F);
    current[2]=-current[0]-current[1];
    Istator[0]=0.5F*current[0]-current[1];
    Istator[1]=-SQRT3_2*current[0];
    Irotor[0]= cosf(position.angle)*Istator[0]+sinf(position.angle)*Istator[1];
    Irotor[1]=-sinf(position.angle)*Istator[0]+cosf(position.angle)*Istator[1];

    if(	(output[0]=float(sqrt(3)/3)*Vout[1]+Vout[0])>=0 &&
	(output[1]=float(2/sqrt(3))*Vout[1])>=0
    ) {
	output[2]=0;
    } else if(
	(output[1]=-Vout[0]+float(sqrt(3)/3)*Vout[1])>=0 &&
	(output[2]=-float(sqrt(3)/3)*Vout[1]-Vout[0])>=0
    ) {
	output[0]=0;
    } else {
	output[0]=Vout[0]-float(sqrt(3)/3)*Vout[1];
	output[1]=0;
	output[2]=-float(2/sqrt(3))*Vout[1];
    }

    if(putp<1000)
	buffer[putp++]=current[0];

    HRTIM1->HRTIM_TIMERx[0].CMP1xR=FULL_SCALE*(0.5F-output[0]);
    HRTIM1->HRTIM_TIMERx[0].CMP2xR=FULL_SCALE*(0.5F+output[0]);
    HRTIM1->HRTIM_TIMERx[0].CMP3xR=FULL_SCALE*(0.5F-output[1]);
    HRTIM1->HRTIM_TIMERx[0].CMP4xR=FULL_SCALE*(0.5F+output[1]);
    HRTIM1->HRTIM_TIMERx[1].CMP1xR=FULL_SCALE*(0.5F-output[2]);
    HRTIM1->HRTIM_TIMERx[1].CMP2xR=FULL_SCALE*(0.5F+output[2]);

    switch(counter) {
    case 0: counter=2000; LED0=1; break;
    case 1000: LED0=0;
    default:
	counter--;
    }
    LED2=1;
}


RCC_ClocksTypeDef clock_info;

int main()
{
    /* First initalize clocks */
    SystemCoreClockUpdate();
    RCC_GetClocksFreq(&clock_info);
    RCC->AHBENR|=
	RCC_AHBENR_DMA1EN |
	RCC_AHBENR_ADC12EN |
	RCC_AHBENR_GPIOBEN |
	RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR|=
	RCC_APB1ENR_USART2EN;
    RCC->APB2ENR|=
	RCC_APB2ENR_HRTIM1;
    RCC->CFGR2|=
	RCC_CFGR2_ADCPRE12_DIV1;
    RCC->CFGR3|=
	RCC_CFGR3_HRTIM1SW;	// PLL clock 64MHz

    /* Initialize pins */
    LED3=LED2=LED1=LED0=1;
    HBEN=0;
    gpio_init(GPIOA,{
	{PP, NONE, SLOW, AF1},	// PA0, TIM2_CH1 linear encoder A
	{PP, NONE, SLOW, AF1},	// PA1, TIM2_CH2 linear encoder B
	{PP, NONE, SLOW, AF1},	// PA2, TIM2_CH3 linear encoder Z
	{PP, NONE, SLOW, ANA},	// PA3, ADC1_IN4 Current 0
	{PP, NONE, SLOW, ANA},	// PA4, ADC2_IN1 Current 1
	{PP, NONE, SLOW, ANA},	// PA5, ADC2_IN2 Current 2
	{PP, NONE, SLOW, OUT},	// PA6, LED0
	{PP, NONE, SLOW, OUT},	// PA7, LED1
	{PP, NONE, FAST, AF13},	// PA8, HRTIM1_CHA1 HB0
	{PP, NONE, FAST, AF13},	// PA9, HRTIM1_CHA2 HB1
	{PP, NONE, FAST, AF13},	// PA10, HRTIM1_CHB1 HB2
	{PP, NONE, SLOW, OUT},	// PA11, HBEN
	{PP, NONE, SLOW, AF7},  // PA12, USART1_RTS_DE host communication
	{PP, PULU, FAST, AF0},	// PA13, SWDAT
	{PP, PULD, FAST, AF0},	// PA14, SWCLK
	{PP, PULU, MEDI, AF7}}	// PA15, USART2_RXD motor encoder
    );
    gpio_init(GPIOB,{
	{PP, NONE, SLOW, OUT},	// PB0, LED2
	{PP, NONE, SLOW, OUT},	// PB1, LED3
	{PP, NONE, SLOW, INP},	// PB2, nc
	{PP, NONE, FAST, AF7},	// PB3, USART2_TXD motor encoder
	{PP, PULU, SLOW, AF0},	// PB4, NJTRST (not used)
	{PP, NONE, SLOW, INP},	// PB5, unused, connected to DXD
	{PP, NONE, SLOW, AF7},	// PB6, USART1_TX host communication
	{PP, NONE, SLOW, AF7},	// PB7, USART1_RX host communication
	{PP, NONE, SLOW, INP},	// PB8, nc
	{PP, NONE, SLOW, INP},	// PB9, nc
	{PP, NONE, SLOW, INP},	// PB10, nc
	{PP, NONE, SLOW, INP},	// PB11, nc
	{PP, NONE, SLOW, INP},	// PB12, nc
	{PP, NONE, SLOW, INP},	// PB13, nc
	{PP, NONE, SLOW, INP},	// PB14, nc
	{PP, NONE, SLOW, INP}}	// PB15, nc
    );

    init_adc();

    // Channel used by HRTIM1A (transmit encoder trigger)
    DMA1_Channel3->CCR=0;	// Disable channel
    DMA1_Channel3->CNDTR=1;
    DMA1_Channel3->CPAR=(__IO uint32_t)&(USART2->TDR);
    DMA1_Channel3->CMAR=(__IO uint32_t)tx_buffer;
    DMA1_Channel3->CCR=
	DMA_CCR_MINC |
	DMA_CCR_CIRC |
	DMA_CCR_DIR |
	DMA_CCR_EN;

    // HRTIM1 PLL VCO 128MHz and recalibrate every 14us
    HRTIM1->HRTIM_COMMON.DLLCR=HRTIM_DLLCR_CAL;
    while(!(HRTIM1->HRTIM_COMMON.ISR & HRTIM_ISR_DLLRDY))
	;
    HRTIM1->HRTIM_COMMON.DLLCR=HRTIM_DLLCR_CALEN | HRTIM_DLLCR_CALRTE;

    HRTIM1->HRTIM_MASTER.MCR&=~(HRTIM_MCR_TACEN | HRTIM_MCR_TBCEN |
	HRTIM_MCR_CK_PSC);
    HRTIM1->HRTIM_MASTER.MCR|=PRESCALE*HRTIM_MCR_CK_PSC_0;

    // Use timer 0 and 1 (A and B)
    for(int x=0;x<2;x++) {
	HRTIM1->HRTIM_TIMERx[x].CNTxR=0;
	HRTIM1->HRTIM_TIMERx[x].PERxR=FULL_SCALE;
	HRTIM1->HRTIM_TIMERx[x].REPxR=3;
	HRTIM1->HRTIM_TIMERx[x].OUTxR=0;
	HRTIM1->HRTIM_TIMERx[x].TIMxCR=
	    HRTIM_TIMCR_PREEN | 
	    HRTIM_TIMCR_CONT | 
	    HRTIM_TIMCR_TREPU |
	    PRESCALE*HRTIM_TIMCR_CK_PSC_0;
    }

    HRTIM1->HRTIM_TIMERx[0].TIMxDIER=
	HRTIM_TIMDIER_REPIE |
	HRTIM_TIMDIER_REPDE;
    HRTIM1->HRTIM_TIMERx[1].TIMxDIER=
	HRTIM_TIMDIER_RST2IE;

    HRTIM1->HRTIM_TIMERx[0].CMP1xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[0].CMP2xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[0].CMP3xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[0].CMP4xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[1].CMP1xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[1].CMP2xR=FULL_SCALE/2;
    HRTIM1->HRTIM_TIMERx[1].CMP3xR=FULL_SCALE-12000;
    HRTIM1->HRTIM_TIMERx[0].SETx1R=HRTIM_SET1R_CMP1;
    HRTIM1->HRTIM_TIMERx[0].RSTx1R=HRTIM_RST1R_CMP2;
    HRTIM1->HRTIM_TIMERx[0].SETx2R=HRTIM_SET2R_CMP3;
    HRTIM1->HRTIM_TIMERx[0].RSTx2R=HRTIM_RST2R_CMP4;
    HRTIM1->HRTIM_TIMERx[1].SETx1R=HRTIM_SET1R_CMP1;
    HRTIM1->HRTIM_TIMERx[1].RSTx1R=HRTIM_RST1R_CMP2;
    //HRTIM1->HRTIM_TIMERx[1].SETx2R=HRTIM_SET2R_PER; setting occurs in timerA repetition interrupt
    HRTIM1->HRTIM_TIMERx[1].RSTx2R=HRTIM_RST2R_CMP3;
    HRTIM1->HRTIM_COMMON.ADC2R=HRTIM_ADC2R_AD2TAPER;

    HRTIM1->HRTIM_MASTER.MCR|=HRTIM_MCR_TACEN | HRTIM_MCR_TBCEN |
	PRESCALE*HRTIM_MCR_CK_PSC_0;
    HRTIM1->HRTIM_COMMON.OENR=
	HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN |
	HRTIM_OENR_TB1OEN;

    NVIC_EnableIRQ(HRTIM1_TIMA_IRQn);
    NVIC_EnableIRQ(HRTIM1_TIMB_IRQn);

    /* Usart2 is used for the encoder also using DMA1-channel6 */
    USART2->CR1=0;
    USART2->CR2=0;
    USART2->CR3=USART_CR3_DMAR;
    USART2->BRR=0x19;
    USART2->CR1=USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    // Channel used by USART2 (receive encoder data)
    DMA1_Channel6->CCR=0;	// Disable channel
    DMA1_Channel6->CNDTR=sizeof(rx_buffer);
    DMA1_Channel6->CPAR=(__IO uint32_t)&(USART2->RDR);
    DMA1_Channel6->CMAR=(__IO uint32_t)rx_buffer;
    DMA1_Channel6->CCR=
	DMA_CCR_MINC |
	DMA_CCR_CIRC |
	DMA_CCR_EN;

    HBEN=1;
    for(int x=0;;x++) {
	LED1=1;
	usleep(100000);
	LED1=0;
	usleep(100000);
    }
}

extern "C" {
void _exit(int a) { for(;;) ; }
void _kill(int a) { for(;;) ; }
void _sbrk(int a) { for(;;) ; }
int _getpid() { return 1; }
}


extern "C" void SystemInit(void)
{
    SCB->CPACR|=15<<20; // FPU for all

    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY_1;

    RCC->CFGR|=
	RCC_CFGR_HPRE_DIV1 |
	RCC_CFGR_PPRE2_DIV1 |
	RCC_CFGR_PPRE1_DIV1 |
	RCC_CFGR_PLLSRC_HSI_Div2 |
	RCC_CFGR_PLLXTPRE_PREDIV1 |
	RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;			// Turn PLL on
    while(!(RCC->CR & RCC_CR_PLLRDY))		// Wait until PLL ready
	;
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;	// Use PLL as system clock

    SCB->VTOR=FLASH_BASE; // Interrupt vectors in flash
}

#undef USART2
extern volatile USART_TypeDef USART2;
#undef ADC1
extern volatile ADC_TypeDef ADC1;
#undef ADC2
extern volatile ADC_TypeDef ADC2;
#undef ADC1_2
extern volatile ADC_Common_TypeDef ADC1_2;

void link(void)
{
    USART2;
    ADC1;
    ADC2;
    ADC1_2;
}
