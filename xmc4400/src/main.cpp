#include <atomic>

#include "gpio.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu8.h"
#include "pwm_3phase.h"

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
#endif

extern "C" void SysTick_Handler(void)
{
    counter++;
}

void CCU80_1_IRQHandler(void)
{
    counter++;
}

icmpProcessing icmp;
Ethernet eth0(
    0x000319450000ULL, 0, 
    RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
    &icmp
);


extern ETH_GLOBAL_TypeDef eth_debug;
int main()
{

    SysTick_Config(SystemCoreClock/1000);

    XMC_CCU8_SetModuleClock(HB0, XMC_CCU8_CLOCK_SCU);
    XMC_CCU8_EnableModule(HB0);
    XMC_CCU8_Init(HB0, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

    XMC_CCU8_SLICE_COMPARE_CONFIG_t g_capcom_slice_object;
    g_capcom_slice_object.tc = 0x00000401;// 00 0000 0000 0100 0000 0001,
    g_capcom_slice_object.psl = 0;
    g_capcom_slice_object.chc = 0;
    g_capcom_slice_object.prescaler_initval = 0;
    g_capcom_slice_object.dither_limit = 0;
    g_capcom_slice_object.float_limit = 0;
    XMC_CCU8_SLICE_CompareInit(HB0, &g_capcom_slice_object);
    XMC_CCU8_SLICE_CompareInit(HB1, &g_capcom_slice_object);
    XMC_CCU8_SLICE_CompareInit(HB2, &g_capcom_slice_object);
    uint32_t pwm_period=SystemCoreClock/2/18000; // 18kHz
    XMC_CCU8_SLICE_SetTimerPeriodMatch(HB0, pwm_period);
    XMC_CCU8_SLICE_SetTimerPeriodMatch(HB1, pwm_period);
    XMC_CCU8_SLICE_SetTimerPeriodMatch(HB2, pwm_period);


    XMC_CCU8_StartPrescaler(HB0);
    XMC_CCU8_EnableClock(HB0, HB0);
    XMC_CCU8_EnableClock(HB0, HB1);
    XMC_CCU8_EnableClock(HB0, HB2);
    HB1=100;

#if 0
    XMC_CCU8_SLICE_SetEvent(HB0, 
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1);
    XMC_CCU8_SLICE_EnableEvent(HB0,
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1);
    /* Connect compare match event to SR1 */
    XMC_CCU8_SLICE_SetInterruptNode(HB0, 
	XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1, XMC_CCU8_SLICE_SR_ID_1);

    /* Enable IRQ */
    NVIC_SetPriority(CCU80_1_IRQn, 
	NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_EnableIRQ(CCU80_1_IRQn);
#endif

    XMC_CCU8_SLICE_StartTimer(HB0);
    XMC_CCU8_SLICE_StartTimer(HB1);
    XMC_CCU8_SLICE_StartTimer(HB2);


    int t=0;
    while (1) {
        if(counter>500) {
            counter-=500;
            LED1^=1;
            HB0=t&1? 2270:2000;
            HB2=t&1? 1500:2000;
            t++;
	    XMC_CCU8_EnableShadowTransfer(HB0, 0x1111);
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
