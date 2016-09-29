#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "misc.h"

void init_adc(void)
{
    ADC1_2->CCR|=ADC12_CCR_MULTI_0*5; // Both ADCs work simultaneously.
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

    // Initialise ADC2
    ADC2->CR&=~ADC_CR_ADVREGEN;
    ADC2->CR|=ADC_CR_ADVREGEN_0;
    usleep(12);
    ADC2->CR|=ADC_CR_ADCAL;
    while((ADC2->CR)&ADC_CR_ADCAL)
	;
    ADC2->CR|=ADC_CR_ADCALDIF | ADC_CR_ADCAL;
    while(ADC2->CR&ADC_CR_ADCAL)
	;
    usleep(1);
    ADC2->SMPR1|=ADC_SMPR1_SMP1_1;
    ADC2->CR|=ADC_CR_ADCAL;
    ADC2->CR|=ADC_CR_ADEN;

    /*
	ADC1_IN4	Current 0
	ADC2_IN1	Current 1
	ADC2_IN2	Current 2
    */
    ADC1->JSQR=
	(ADC_JSQR_JSQ1_0 * 4) |
	(ADC_JSQR_JSQ2_0 * 1) |
	(ADC_JSQR_JEXTEN_0) |		// Rising edge
	(ADC_JSQR_JEXTSEL_0 * 9) | 	// HRTIM_ADCTRG2 event (pg 227)
	(ADC_JSQR_JL_0 * 1);		// Two conversions
    ADC2->JSQR=
	(ADC_JSQR_JSQ1_0 * 1) |
	(ADC_JSQR_JSQ2_0 * 2) |
	(ADC_JSQR_JL_0 * 1);		// One conversion
    ADC1->CR|=ADC_CR_JADSTART;
}
