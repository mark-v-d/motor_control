#include "uart.h"
#include "vadc.h"
#include "ccu4.h"
#include "crc.h"
#include <array>

using namespace std::chrono_literals;

gpio::output<0,0> DAC;
gpio::pin<0,14> RXD;
gpio::pin<0,15> TXD;
gpio::pin<2,0> I0_P;
gpio::pin<2,11> I1_P;
gpio::pin<2,8> I2_P;
gpio::pin<2,10> V;
gpio::pin<0,9> RXD_TIMER;

uart::full_duplex serial(TXD,RXD);
ccu4::edge_capture<0,3> serial_capture;

extern "C" void USIC0_0_IRQHandler(void)
{
    static_assert(serial.UNIT==0, "Wrong uart");

    adc::queue<0>(I0_P,	adc::EXTERNAL_TRIGGER);
    adc::queue<0>(V, adc::ENSI);
    adc::queue<1>(I1_P,	adc::EXTERNAL_TRIGGER);
    adc::queue<1>(I2_P, 0);

    serial->PSCR=USIC_CH_PSCR_CRIF_Msk;
}

extern "C" void VADC0_G0_0_IRQHandler(void)
{
    std::array<uint16_t,5> data;
    serial.tx_fifo(data[0]=adc::vadc.G[0].RES[0]);
    serial.tx_fifo(data[1]=adc::vadc.G[1].RES[0]);
    serial.tx_fifo(data[2]=adc::vadc.G[0].RES[1]);
    serial.tx_fifo(data[3]=adc::vadc.G[1].RES[1]);
    serial.tx_fifo(data[4]=adc::vadc.G[0].SEFLAG | (adc::vadc.G[1].SEFLAG<<8));
    serial.tx_fifo(crc::ccitt_16(data).get());
    adc::vadc.G[0].SEFCLR=VADC_G_SEFCLR_SEV0_Msk;
    adc::vadc.G[1].SEFCLR=VADC_G_SEFCLR_SEV0_Msk;
}

int main(int argc, char **argv)
{
    DAC.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    ////////////////////////////////////////////////////////////////////////////
    // UART
    ////////////////////////////////////////////////////////////////////////////
    serial.init(uart::Baudrate(4e6));
    uart::fifo_configure<16,8>(serial);
    serial.enable_rx_interrupt<0>();
    NVIC_EnableIRQ(serial.irq<0>());

    ////////////////////////////////////////////////////////////////////////////
    // Timer used to trigger the ADC
    ////////////////////////////////////////////////////////////////////////////
    ccu4::init<0>(
	XMC_CCU4_CLOCK_SCU,
	XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR
    );

    serial_capture.init();
    //serial_capture.period(1s/20000.0f);
    serial_capture.set_event<1>(RXD_TIMER,ccu4::EDGE_FALLING);
    serial_capture.service_request_event<1>(2); // trigger ADC
    //serial_capture.enable_capture<0>(1);

    ccu4::start(serial_capture);
    ccu4::shadow_transfer(serial_capture);

    ////////////////////////////////////////////////////////////////////////////
    // ADC
    ////////////////////////////////////////////////////////////////////////////
    adc::init();
    adc::global_class<0>(XMC_VADC_CONVMODE_12BIT,15);
    adc::channel_control<0>(I0_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<0>(V,	  XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::channel_control<1>(I1_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<1>(I2_P, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::queue_config<0>( XMC_VADC_GATEMODE_IGNORE, 0,
	XMC_VADC_REQ_TR_CCU40_SR2, XMC_VADC_TRIGGER_EDGE_RISING
    );
    adc::queue_config<1>( XMC_VADC_GATEMODE_IGNORE, 0,
	XMC_VADC_REQ_TR_CCU40_SR2, XMC_VADC_TRIGGER_EDGE_RISING
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

    NVIC_EnableIRQ(VADC0_G0_0_IRQn);

    for(;;) {
    }
    return 0;
}
