#include "gpio.h"

namespace usic {

enum pin_function_t {
    DX0A, DX1A, DX2A, DX3A, DX4A, DX5A, // inputs
    DX0B, DX1B, DX2B, DX3B, DX4B, DX5B,
    DX0C, DX1C, DX2C, DX3C, DX4C, DX5C,
    DX0D, DX1D, DX2D, DX3D, DX4D, DX5D,
    DX0E, DX1E, DX2E, DX3E, DX4E, DX5E,
    OUTPUT
};

struct info_t {
    int unit;
    int channel;
    pin_function_t function;
    XMC_GPIO_MODE gpio_mode;
};

#include "usic_map.h"

constexpr location_t<XMC_USIC_CH_t> location(info_t i)
{
    if(i.unit==0) {
	if(i.channel==0)
	    return USIC0_CH0_BASE;
	else
	    return USIC0_CH1_BASE;

    } else {
	if(i.channel==0)
	    return USIC1_CH0_BASE;
	else
	    return USIC1_CH1_BASE;
    }
}


template<int f,int PORT,int PIN>
constexpr int dx(gpio::pin<PORT,PIN> const&)
{
    constexpr int x=dx0<PORT,PIN>().function;
    if constexpr(x==DX0A) return 0;
    if constexpr(x==DX0B) return 1;
    if constexpr(x==DX0C) return 2;
    if constexpr(x==DX0D) return 3;
    if constexpr(x==DX0E) return 4;
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Full duplex serial, no interrupts
////////////////////////////////////////////////////////////////////////////////
template <class TX_PIN, class RX_PIN>
class full_duplex_asc {
    constexpr static auto channel=location(dout0(TX_PIN{}));

public:
    full_duplex_asc(void) {
	static_assert(
	    uint32_t(location(dout0(TX_PIN{})))
	    ==uint32_t(location(dx0(RX_PIN{}))),
	    "TXD and RXD not on the same channel");
    }

    XMC_USIC_CH_t* operator->() const { return channel; }

    void init(uint32_t brd) {
	XMC_UART_CH_CONFIG_t uart_config = {
	    .baudrate=brd,
	    .data_bits=8U,
	    .frame_length=0,
	    .stop_bits=1,
	    .oversampling=0,
	    .parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
	};

	TX_PIN{}.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | dout0(TX_PIN{}).gpio_mode);
	TX_PIN{}.set(XMC_GPIO_HWCTRL_DISABLED);

	//XMC_UART_CH_DisableEvent(channel, XMC_UART_CH_EVENT_FRAME_FINISHED);
	//NVIC_DisableIRQ(irq<hd_irq>(ENC_TXD));

	XMC_UART_CH_Init(channel, &uart_config);
	XMC_UART_CH_SetInputSource(channel,XMC_UART_CH_INPUT_RXD,dx<0>(RX_PIN{}));
	//XMC_UART_CH_EnableEvent(channel, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
	XMC_UART_CH_Start(channel);
    }
    void disable(void) {
	// Reset, resets both channels so don't
	//XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	//XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	//NVIC_DisableIRQ(USIC1_0_IRQn);
	//NVIC_DisableIRQ(USIC1_1_IRQn);
    }

    void tx(uint8_t d) {
	while(channel->TCSR & XMC_USIC_CH_TBUF_STATUS_BUSY)
	    ;
	channel->PSCR=USIC_CH_PSR_ASCMode_TBIF_Msk;
	channel->TBUF[0]=d;
    }

    template <class T>
    int rx(T time)
    {
	std::chrono::duration<float,std::ratio<1,frequency>> ticks=time;
	systick.LOAD=ticks.count();
	systick.VAL=0;
	while(!(systick.CTRL&0x10000))
	    if(channel->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk
		| USIC_CH_RBUFSR_RDV1_Msk)
	    )
		return channel->RBUF;
	return -1;
    }
};


template <class TX_PIN, class RX_PIN>
full_duplex_asc<TX_PIN,RX_PIN> make_full_duplex_asc(TX_PIN tx,RX_PIN rx)
{
    return full_duplex_asc<TX_PIN,RX_PIN>{};
}

}
