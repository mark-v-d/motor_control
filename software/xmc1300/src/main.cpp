#include "gpio.h"
#include "uart.h"
#include "vadc.h"
#include "ccu8.h"
#include <initializer_list>

using namespace std::chrono_literals;

gpio::output<0,0> DAC;
gpio::pin<0,14> RXD;
gpio::pin<0,15> TXD;
gpio::pin<2,0> I0_P;
gpio::pin<2,6> I0_N;
gpio::pin<2,11> I1_P;
gpio::pin<2,10> I1_N;
gpio::pin<2,8> I2_P;
//gpio::pin<2,9> I2_N;
gpio::pin<2,9> V;
gpio::pin<0,9> RXD_TIMER;


uart::full_duplex_no_int serial(TXD,RXD);

extern "C" void SysTick_Handler(void)
{
    static uint8_t i;
    DAC.toggle();
}

extern "C" void USIC0_0_IRQHandler(void)
{
    static_assert(serial.UNIT==0, "Wrong uart");
    uint16_t data=adc::vadc.G[0].RES[1]&0xffff;

    serial.tx_fifo(data);
    serial->PSCR=USIC_CH_PSCR_CRIF_Msk;
}

int main(int argc, char **argv)
{
    DAC.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    SysTick_Config(1000000);
    //NVIC_SetPriorityGrouping(0);
    //NVIC_SetPriority(SysTick_IRQn,0);
    NVIC_DisableIRQ(SysTick_IRQn);

    uart::fifo_configure<0,0,8,0>(serial,serial);

    serial.init(1e6);
    serial.enable_rx_interrupt<0>();
    NVIC_EnableIRQ(serial.irq<0>());

    adc::init();
    adc::global_class<0>(XMC_VADC_CONVMODE_12BIT, 0);

    adc::channel_control<0>(I0_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<0>(V,	  XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::channel_control<1>(I1_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<1>(I2_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::queue<0>(I0_P,	adc::REFILL);// | adc::EXTERNAL_TRIGGER);
    adc::queue<0>(V,	adc::REFILL);
    adc::queue<1>(I1_P,	adc::REFILL | adc::EXTERNAL_TRIGGER);
    adc::queue<1>(I2_P,	adc::REFILL);

    ccu8::init<0>(
	XMC_CCU8_CLOCK_SCU,
	XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR
    );

    adc::queue_config<0>( 0, // Use channel result register
	XMC_VADC_GATEMODE_IGNORE,
	XMC_VADC_REQ_TR_CCU80_SR2, XMC_VADC_TRIGGER_EDGE_RISING
    );
    adc::queue_config<1>( 0, // Use channel result register
	XMC_VADC_GATEMODE_IGNORE,
	XMC_VADC_REQ_TR_CCU80_SR2, XMC_VADC_TRIGGER_EDGE_RISING
    );

    for(int i: {0,1}) {
	adc::vadc.G[i].ARBPR=
	    bitfield<VADC_G_ARBPR_PRIO0_Msk>(3) |
	    VADC_G_ARBPR_CSM0_Msk |
	    VADC_G_ARBPR_ASEN0_Msk;
	adc::vadc.G[i].ARBCFG=
	    bitfield<VADC_G_ARBCFG_ANONC_Msk>(3) |
	    bitfield<VADC_G_ARBCFG_ANONS_Msk>(3);
    }

    for(;;) {
    }
    return 0;
}
