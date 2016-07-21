#include "stm32f30x.h"
#include "stm32f30x_rcc.h"

#define DEAD_TIME 20
#define PRESCALE 2

float counter;

/* encoder 5000Hz,
    0 0101 1000 --> 0x1a
    2.8Mb/s
*/

/*
class GPIO {
    GPIO_TypeDef *p;
    int num;
public:
    GPIO(GPIO_TypeDef *ip, int inum) { p=ip; num=inum; }
    int operator=(int i) { p->BSRR=1<<(num+(i? 0:16)); }

};

GPIO LED0(GPIOA,6);
GPIO LED1(GPIOA,7);
GPIO LED2(GPIOB,0);
GPIO LED3(GPIOB,1);

*/

__attribute__((noinline)) void usleep(uint32_t t)
{
    for(float f=t*6.2301F;f>0;f-=1.0F)
	asm("nop");
}

static enum {
    OFF=0,
    UP,
    DOWN,
    FEED,
    MANUAL
} state=OFF;
float current, voltage;
float up_speed=40000, down_speed=20000;

int16_t store[3000], trace, putp, subsample_ctr, subsample=10;

static struct {
    float P, I; // Gains
    float llimit, ulimit;
    float integrator;
    float output;

    float Vout, Vemk, Vset, Verror;
    float Rmotor;
} pid;
float adc_time;

extern "C" void HRTIM1_TIMA_IRQHandler(void)
{
    static float output;

    HRTIM1->HRTIM_TIMERx[0].TIMxICR=HRTIM_TIMICR_REPC;
    USART2->TDR=0x1a;

    GPIOB->BSRR=GPIO_BSRR_BR_7;
}


RCC_ClocksTypeDef clock_info;
volatile char rx_buffer[8];

int main()
{
    SystemCoreClockUpdate();
    RCC->AHBENR|=
	RCC_AHBENR_DMA1EN |
	RCC_AHBENR_ADC12EN |
	RCC_AHBENR_GPIOBEN |
	RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR|=
	RCC_APB1ENR_USART2EN |
	RCC_APB1ENR_DAC1EN;
    RCC->APB2ENR|=
	RCC_APB2ENR_HRTIM1;
    RCC->CFGR2|=RCC_CFGR2_ADCPRE12_DIV1;
    RCC->CFGR3|=
	RCC_CFGR3_HRTIM1SW;	// PLL clock 64MHz

    RCC_GetClocksFreq(&clock_info);

    // Put PA8, PA9, PA10 and PA11 in HRTIM1_CHxx mode
    GPIOA->AFR[1]&=0xffff0000;
    GPIOA->AFR[1]|=0x0000dddd; // Alternate function 13
    GPIOA->OSPEEDR|=
	GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR10 |
	GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR8;
    GPIOA->MODER&=~(
	GPIO_MODER_MODER5 | GPIO_MODER_MODER6 |	// FIXME debug only
	GPIO_MODER_MODER8 | GPIO_MODER_MODER9 |
	GPIO_MODER_MODER10 | GPIO_MODER_MODER11);
    GPIOA->MODER|=
	GPIO_MODER_MODER0 | 				// Isense input
	GPIO_MODER_MODER4 |				// Analog output
	GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 |	// FIXME debug only
	GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 |	// H-bridge
	GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1;	// H-bridge
    GPIOB->AFR[0]&=0xfff00fff;
    GPIOB->AFR[0]|=0x00077000;				// PB3==USART2
    GPIOB->MODER&=~(
	GPIO_MODER_MODER7 |				// Interrupt scope
	GPIO_MODER_MODER4 | GPIO_MODER_MODER3);		// USART2
    GPIOB->MODER|=(
	GPIO_MODER_MODER7_0 |				// Interrupt scope
	GPIO_MODER_MODER4_1 | GPIO_MODER_MODER3_1);	// USART2 

    DAC1->CR=DAC_CR_EN1 | DAC_CR_BOFF1;

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
    HRTIM1->HRTIM_TIMERx[0].TIMxDIER=HRTIM_TIMDIER_REPIE;	
    HRTIM1->HRTIM_TIMERx[0].CMP2xR=0;
    HRTIM1->HRTIM_COMMON.ADC2R=HRTIM_ADC2R_AD2TAC2;

    HRTIM1->HRTIM_MASTER.MCR|=HRTIM_MCR_TACEN | HRTIM_MCR_TBCEN |
	PRESCALE*HRTIM_MCR_CK_PSC_0;
    HRTIM1->HRTIM_COMMON.OENR=
	HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN |
	HRTIM_OENR_TB1OEN | HRTIM_OENR_TB2OEN ;

    NVIC_EnableIRQ(HRTIM1_TIMA_IRQn);

    USART2->CR1=0;
    USART2->CR2=0;
    USART2->CR3=0;
    USART2->BRR=0x1a;
    USART2->CR1=USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    DMA1_Channel6->CCR=0;	// Disable channel
    DMA1_Channel6->CNDTR=4;
    DMA1_Channel6->CPAR=(__IO uint32_t)&(USART2->RDR);
    DMA1_Channel6->CMAR=(__IO uint32_t)rx_buffer;
    DMA1_Channel6->CCR=
	DMA_CCR_MINC |
	DMA_CCR_CIRC |
	DMA_CCR_EN;

    GPIOA->BSRR=((1<<6) | (1<<7))<<16;

    for(int x=0;;x++) {
    }
}

void test(int x)
{
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
