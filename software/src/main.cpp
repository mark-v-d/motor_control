#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "gpio_init.h"

#define DEAD_TIME 20
#define PRESCALE 2

/* encoder 5000Hz,
    0 0101 1000 --> 0x1a
    2.8Mb/s
*/
char tx_buffer[1]={ 0x1a };
volatile char rx_buffer[9];
struct position_t {
    uint8_t start;
    uint8_t state;
    uint32_t angle;
    uint32_t rotation;
    uint8_t crc;

    position_t(void) {}
    position_t(volatile char *p) {
	start=p[0];
	state=p[1];
	angle=p[2]+256*p[3]+65536*p[4];
	rotation=p[5]+0x100*p[6]+0x10000*p[7];
	crc=p[8];
    }
} position;

__attribute__((noinline)) void usleep(uint32_t t)
{
    for(float f=t*6.2301F;f>0;f-=1.0F)
	asm("nop");
}


extern "C" void HRTIM1_TIMA_IRQHandler(void)
{
    static int counter=0;

    HRTIM1->HRTIM_TIMERx[0].TIMxICR=HRTIM_TIMICR_REPC;
    position=rx_buffer;

    switch(counter) {
    case 0: counter=2000; LED0=1; break;
    case 1000: LED0=0;
    default:
	counter--;
    }
}


RCC_ClocksTypeDef clock_info;

int main()
{
    /* First initalize clocks */
    SystemCoreClockUpdate();
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
    RCC_GetClocksFreq(&clock_info);

    /* Initialize pins */
    LED3=LED2=LED1=LED0=1;
    HBEN=0;
    gpio_init(GPIOA,{
	{PP, NONE, SLOW, AF1},	// PA0, TIM2_CH1
	{PP, NONE, SLOW, AF1},	// PA1, TIM2_CH2
	{PP, NONE, SLOW, AF1},	// PA2, TIM2_CH3
	{PP, NONE, SLOW, ANA},	// PA3, ADC1_IN4
	{PP, NONE, SLOW, ANA},	// PA4, ADC2_IN1
	{PP, NONE, SLOW, ANA},	// PA5, ADC2_IN2
	{PP, NONE, SLOW, OUT},	// PA6, LED0
	{PP, NONE, SLOW, OUT},	// PA7, LED1
	{PP, NONE, FAST, AF13},	// PA8, HRTIM1_CHA1
	{PP, NONE, FAST, AF13},	// PA9, HRTIM1_CHA2
	{PP, NONE, FAST, AF13},	// PA10, HRTIM1_CHB1
	{PP, NONE, SLOW, OUT},	// PA11, HBEN
	{PP, NONE, SLOW, AF7},  // PA12, USART1_RTS_DE
	{PP, PULU, FAST, AF0},	// PA13, SWDAT
	{PP, PULD, FAST, AF0},	// PA14, SWCLK
	{PP, PULU, MEDI, AF7}}	// PA15, USART2_RXD
    );
    gpio_init(GPIOB,{
	{PP, NONE, SLOW, OUT},	// PB0, LED2
	{PP, NONE, SLOW, OUT},	// PB1, LED3
	{PP, NONE, SLOW, INP},	// PB2, nc
	{PP, NONE, FAST, AF7},	// PB3, USART2_TXD
	{PP, PULU, SLOW, AF0},	// PB4, NJTRST (not used)
	{PP, NONE, SLOW, INP},	// PB5, unused, connected to DXD
	{PP, NONE, SLOW, AF7},	// PB6, USART1_TX
	{PP, NONE, SLOW, AF7},	// PB7, USART1_RX
	{PP, NONE, SLOW, INP},	// PB8, nc
	{PP, NONE, SLOW, INP},	// PB9, nc
	{PP, NONE, SLOW, INP},	// PB10, nc
	{PP, NONE, SLOW, INP},	// PB11, nc
	{PP, NONE, SLOW, INP},	// PB12, nc
	{PP, NONE, SLOW, INP},	// PB13, nc
	{PP, NONE, SLOW, INP},	// PB14, nc
	{PP, NONE, SLOW, INP}}	// PB15, nc
    );

    // Initialise ADC1
    ADC1->CR&=~ADC_CR_ADVREGEN;
    ADC1->CR|=ADC_CR_ADVREGEN_0;
    usleep(12);
    ADC1->CR|=ADC_CR_ADCAL;
    while((ADC1->CR)&ADC_CR_ADCAL)
	;
    ADC1->CR|=ADC_CR_ADCALDIF | ADC_CR_ADCAL;
    while(ADC1->CR&ADC_CR_ADCAL)
	;
    usleep(1);
    ADC1->SMPR1|=ADC_SMPR1_SMP1_1;
    ADC1->CR|=ADC_CR_ADCAL;
    ADC1->CR|=ADC_CR_ADEN;

    /*
	ADC1_IN1	Isense
	ADC1_IN4	12V
	ADC1_IN11	to spare connector
	ADC1_IN12	Speed pot
    */
    ADC1->JSQR=
	(ADC_JSQR_JSQ1_0 * 1) |
	(ADC_JSQR_JSQ2_0 * 4) |
	(ADC_JSQR_JSQ3_0 * 11) |
	(ADC_JSQR_JSQ4_0 * 12) |
	(ADC_JSQR_JEXTEN_0) |		// Rising edge
	(ADC_JSQR_JEXTSEL_0 * 9) | 	// HRTIM_ADCTRG2 event (pg 227)
	(ADC_JSQR_JL_0 * 3);
    ADC1->CR|=ADC_CR_JADSTART;

    /*
	ADC2_IN2	Opamp - (AC current)
	ADC2_IN3	Opamp out (AC current)
	ADC2_IN4	Opamp + (AC current)
    */


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

    // 50% duty cycle with dead time insertion on CHA1 and CHA2
    for(int x=0;x<2;x++) {
	HRTIM1->HRTIM_TIMERx[x].CNTxR=0;
	HRTIM1->HRTIM_TIMERx[x].PERxR=0xde00;
	HRTIM1->HRTIM_TIMERx[x].DTxR=
	    HRTIM_DTR_SDTF |
	    HRTIM_DTR_SDTR |
	    HRTIM_DTR_DTF&(DEAD_TIME*HRTIM_DTR_DTF_0) |
	    HRTIM_DTR_DTR&(DEAD_TIME*HRTIM_DTR_DTR_0);
	HRTIM1->HRTIM_TIMERx[x].SETx1R=HRTIM_SET1R_PER;
	HRTIM1->HRTIM_TIMERx[x].RSTx1R=HRTIM_RST1R_CMP1;
	HRTIM1->HRTIM_TIMERx[x].OUTxR=
	    HRTIM_OUTR_POL2 |
	    HRTIM_OUTR_POL1 |
	    HRTIM_OUTR_DTEN;
	HRTIM1->HRTIM_TIMERx[x].REPxR=3;
	HRTIM1->HRTIM_TIMERx[x].TIMxCR=
	    HRTIM_TIMCR_PREEN | 
	    HRTIM_TIMCR_CONT | 
	    HRTIM_TIMCR_TREPU |
	    PRESCALE*HRTIM_TIMCR_CK_PSC_0;
    }
    HRTIM1->HRTIM_TIMERx[0].TIMxDIER=
	HRTIM_TIMDIER_REPIE |
	HRTIM_TIMDIER_REPDE;

    HRTIM1->HRTIM_TIMERx[0].CMP2xR=0;
    HRTIM1->HRTIM_COMMON.ADC2R=HRTIM_ADC2R_AD2TAC2;

    HRTIM1->HRTIM_MASTER.MCR|=HRTIM_MCR_TACEN | HRTIM_MCR_TBCEN |
	PRESCALE*HRTIM_MCR_CK_PSC_0;
    HRTIM1->HRTIM_COMMON.OENR=
	HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN |
	HRTIM_OENR_TB1OEN | HRTIM_OENR_TB2OEN ;

    NVIC_EnableIRQ(HRTIM1_TIMA_IRQn);

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

    for(int x=0;;x++) {
	LED1=1;
	usleep(200000);
	LED1=0;
	usleep(200000);
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

void link(void)
{
    USART2;
}
