#include "XMC4500.h"

#define  PIN_INPUT      0x00
#define  PIN_INPUT_PD   0x01
#define  PIN_INPUT_PU   0x02
#define  PIN_OUT_PP     0x10
#define  PIN_OUT_OD     0x18
#define  PIN_ANALOG     0xFF

volatile uint32_t dummy32;

void pinMode(uint8_t port, uint8_t pin, uint8_t pinmode)
{
    __IO uint32_t *IOCRx =
        (__IO uint32_t *) (PORT0_BASE + 0x010 + port * 0x100 + (pin / 4) * 4);
    __IO uint32_t *PDISC =
        (__IO uint32_t *) (PORT0_BASE + 0x060 + port * 0x100);
    if (pinmode != PIN_ANALOG) {
        uint8_t pos = 3 + ((pin % 4) << 3);
        *IOCRx = (*IOCRx & ~(0x1FUL << pos)) | (pinmode << pos);
        if (port == 14)
            *PDISC = *PDISC & ~(0x1UL << pin);
        if (port == 15)
            *PDISC = *PDISC & ~(0x1UL << pin);
    } else {
        if (port == 14)
            *PDISC = *PDISC | (0x1UL << pin);
        if (port == 15)
            *PDISC = *PDISC | (0x1UL << pin);
    }
}

uint32_t readPin(uint8_t port, uint8_t pin)
{
    __IO uint32_t *PIN =
        (__IO uint32_t *) ((uint32_t) (&PORT0->IN) + port * 0x100);
    return ((*PIN) >> pin) & 1;
}

void writePin(uint8_t port, uint8_t pin, uint32_t value)
{
    __IO uint32_t *POMR =
        (__IO uint32_t *) ((uint32_t) (&PORT0->OMR) + port * 0x100);
    uint32_t omrvalue = (value) ? (1 << pin) : (1 << (16 + pin));
    *POMR = omrvalue;
}

void setPin(uint8_t port, uint8_t pin)
{
    __IO uint32_t *POMR =
        (__IO uint32_t *) ((uint32_t) (&PORT0->OMR) + port * 0x100);
    uint32_t omrvalue = 0x1 << pin;
    *POMR = omrvalue;
}

void clearPin(uint8_t port, uint8_t pin)
{
    __IO uint32_t *POMR =
        (__IO uint32_t *) ((uint32_t) (&PORT0->OMR) + port * 0x100);
    uint32_t omrvalue = 0x10000 << pin;
    *POMR = omrvalue;
}

void togglePin(uint8_t port, uint8_t pin)
{
    __IO uint32_t *POMR =
        (__IO uint32_t *) ((uint32_t) (&PORT0->OMR) + port * 0x100);
    uint32_t omrvalue = 0x10001 << pin;
    *POMR = omrvalue;
}

void attachInterrupt(uint8_t port, uint8_t pin, void (*ISR) (void),
                     uint8_t mode)
{
    /* TODO Fix this */
}

/****************************************************
 * 
 * System Timer
 * 
 * **************************************************/

volatile uint32_t tickflag;
volatile uint32_t systick_count;

#ifdef __cplusplus
extern "C"
#endif
void SysTick_Handler(void)
{
    tickflag = 1;
    systick_count++;
}

void initSysTick()
{

    // Use processor clock, enable interrupt request and enable counter
    PPB->SYST_CSR |= 0x7;
    // Set the reload register (timebase in effect)
    PPB->SYST_RVR = SystemCoreClock / 1000 - 1; //generate 1 millisecond time base
    PPB->SYST_CVR = 5;          // Start the counter at a value close to zero
    __enable_irq();
    PORT1->OUT = 1;
}

/*****************************************************
* main
*	setup
*	loop
*****************************************************/

void initPorts()
{
    // LED's are on P1.0 and P1.1
    pinMode(1, 0, PIN_OUT_PP);  /* LED0 */
    pinMode(1, 1, PIN_OUT_PP);  /* LED1 */
}

uint32_t pin_1_14;

int main()
{
    initPorts();
    initSysTick();

    while (1) {
        pin_1_14 = readPin(1, 14);

        /* Flash leds faster when BTN1 is pressed */
        if (tickflag && ((systick_count % (500 + 1500 * pin_1_14)) == 0)) {
            tickflag = 0;
            /* Toggle leds */
            PORT1->OMR = 0x30000 | 0x03;
        };

    };

    return 0;
}

extern "C" void _exit(int) {}
