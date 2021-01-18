#include "gpio.h"

namespace usic {

enum pin_function_t {
    DX0A, DX1A, DX2A, DX3A, DX4A, DX5A, // inputs
    DX0B, DX1B, DX2B, DX3B, DX4B, DX5B,
    DX0C, DX1C, DX2C, DX3C, DX4C, DX5C,
    DX0D, DX1D, DX2D, DX3D, DX4D, DX5D,
    DOUT0, DOUT1, DOUT2, DOUT3,		// outputs
    SELO0, SELO1,
    SCLKOUT
};

struct info_t {
    int unit;
    int channel;
    pin_function_t function;
    XMC_GPIO_MODE gpio_mode;
};

template <int port, int pin>
constexpr info_t info(void)
{
    static_assert(port==-1, "Invalid pin for a USART");
    return info_t{-1};
}

#define INFO template<> constexpr info_t info
INFO<0,0>() { return info_t{1,1,DX0D,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
INFO<0,1>() { return info_t{1,1,DOUT0,	XMC_GPIO_MODE_OUTPUT_ALT2	};}


#if 0
#define USIC0_C0_DX0_P5_0   	3
#define USIC0_C0_DX2_P1_0   	0
#define USIC0_C1_DX0_P4_0   	4
#define USIC0_C1_DX1_P3_0   	1
#define USIC1_C1_DX0_P0_0   	3
#define USIC1_C1_DX1_P4_0   	2
#endif

template <int PORT, int PIN>
constexpr location_t<XMC_USIC_CH_t> channel_location(gpio::pin<PORT,PIN> const&)
{
    constexpr auto i=info<PORT, PIN>();
    static_assert(i.unit==0 || i.unit==1, "Oops");
    static_assert(i.channel==0 || i.channel==1, "Oops");
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

template <int PORT, int PIN>
constexpr pin_function_t function(gpio::pin<PORT,PIN> const&)
{
    return info<PORT,PIN>().function;
}

template <int PORT, int PIN>
constexpr XMC_GPIO_MODE gpio_mode(gpio::pin<PORT,PIN> const&)
{
    return info<PORT,PIN>().gpio_mode;
}

template<int PORT,int PIN>
constexpr int dx0(gpio::pin<PORT,PIN> const&)
{
    constexpr auto x=info<PORT,PIN>().function;
    if constexpr(x==DX0A)
	return 0;
    if constexpr(x==DX0B)
	return 1;
    if constexpr(x==DX0C)
	return 2;
    if constexpr(x==DX0D)
	return 3;
    return -1;
}

template <class TX_PIN, class RX_PIN>
class full_duplex_asc {
    constexpr static auto channel=channel_location(TX_PIN{});

public:
    full_duplex_asc(void) {
	static_assert(uint32_t(channel_location(TX_PIN{}))
	    ==uint32_t(channel_location(RX_PIN{})),
	    "TX and RX not on the same channel");
	static_assert(function(TX_PIN{})==DOUT0, "TX is not tx");
	static_assert(dx0(RX_PIN{})!=-1, "RX is not rx");
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

	TX_PIN{}.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | gpio_mode(TX_PIN{}));
	TX_PIN{}.set(XMC_GPIO_HWCTRL_DISABLED);

	//XMC_UART_CH_DisableEvent(channel, XMC_UART_CH_EVENT_FRAME_FINISHED);
	//NVIC_DisableIRQ(irq<hd_irq>(ENC_TXD));

	XMC_UART_CH_Init(channel, &uart_config);
	XMC_UART_CH_SetInputSource(channel,XMC_UART_CH_INPUT_RXD,dx0(RX_PIN{}));
	XMC_UART_CH_EnableEvent(channel, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
	XMC_UART_CH_Start(channel);
    }
    void disable(void) {
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	NVIC_DisableIRQ(USIC1_0_IRQn);
	NVIC_DisableIRQ(USIC1_1_IRQn);
    }

    bool tx_busy(void) {
	return channel->TCSR & XMC_USIC_CH_TBUF_STATUS_BUSY;
    }
    void tx(uint8_t d) {
	while(channel->TCSR & XMC_USIC_CH_TBUF_STATUS_BUSY)
	    ;
	channel->PSCR=USIC_CH_PSR_ASCMode_TBIF_Msk;
	channel->TBUF[0]=d;
	//XMC_UART_CH_Transmit
    }

    int rx_now(void)
    {
	if(channel->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))
	    return channel->RBUF;
	else
	    return -1;
    }

};


template <class TX_PIN, class RX_PIN>
full_duplex_asc<TX_PIN,RX_PIN> make_full_duplex_asc(TX_PIN tx,RX_PIN rx)
{
    return full_duplex_asc<TX_PIN,RX_PIN>{};
}

}
