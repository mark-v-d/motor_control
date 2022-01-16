#include "gpio.h"
#include "misc.h"

namespace uart {

enum pin_function_t {
    DX0A, DX1A, DX2A, DX3A, DX4A, DX5A, // inputs
    DX0B, DX1B, DX2B, DX3B, DX4B, DX5B,
    DX0C, DX1C, DX2C, DX3C, DX4C, DX5C,
    DX0D, DX1D, DX2D, DX3D, DX4D, DX5D,
    DX0E, DX1E, DX2E, DX3E, DX4E, DX5E,
    DX0F, DX1F, DX2F, DX3F, DX4F, DX5F,
    OUTPUT
};

struct info_t {
    int unit;
    int channel;
    pin_function_t function;
    XMC_GPIO_MODE gpio_mode;
};

#include "uart_map.h"

constexpr location_t<XMC_USIC_CH_t> location(info_t const &i)
{
    if(i.unit==0) {
	if(i.channel==0)
	    return USIC0_CH0_BASE;
	else
	    return USIC0_CH1_BASE;

    } else {
#ifdef USIC1_CH0_BASE
	if(i.channel==0)
	    return USIC1_CH0_BASE;
	else
	    return USIC1_CH1_BASE;
#else
#endif
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



template <int FIFO_SIZE>
constexpr int validate_fifo_size(void) {
    static_assert(FIFO_SIZE==0 || FIFO_SIZE==2 || FIFO_SIZE==4 ||
	FIFO_SIZE==8 || FIFO_SIZE==16 || FIFO_SIZE==32 || FIFO_SIZE==64,
	"Invalid fifo size (valid sizes are 2,4,8,16,32 and 64)");
    return
	(FIFO_SIZE==0? 0:
	(FIFO_SIZE==2? 1:
	(FIFO_SIZE==4? 2:
	(FIFO_SIZE==8? 3:
	(FIFO_SIZE==16? 4:
	(FIFO_SIZE==32? 5:6
    ))))));
}

template <int tx0,int rx0, int tx1, int rx1, typename UART0, typename UART1>
void fifo_configure(UART0 &&ch0, UART1 &&ch1) {
    static_assert(tx0+rx0+tx1+rx1<=64, "Only 64 fifo entries available");
    validate_fifo_size<tx0>();
    validate_fifo_size<rx0>();
    validate_fifo_size<tx1>();
    validate_fifo_size<rx1>();
    static_assert(ch0.UNIT == ch1.UNIT, "Not the same UART");

    ch0->RBCTR=0;
    ch1->RBCTR=0;
    ch0->TBCTR=0;
    ch1->TBCTR=0;

    int base=0;
    for(int pr_size=6, size=64; size>=2; size>>=1, pr_size--) {
	if(tx0>=size) {
	    ch0->TBCTR=
		bitfield<USIC_CH_TBCTR_DPTR_Msk>(base)
		| bitfield<USIC_CH_TBCTR_LIMIT_Msk>(0)
		| bitfield<USIC_CH_TBCTR_SIZE_Msk>(pr_size);
	    base+=size;
	}
	if(tx1>=size) {
	    ch1->TBCTR=
		bitfield<USIC_CH_TBCTR_DPTR_Msk>(base)
		| bitfield<USIC_CH_TBCTR_LIMIT_Msk>(0)
		| bitfield<USIC_CH_TBCTR_SIZE_Msk>(pr_size);
	    base+=size;
	}
	if(rx0>=size) {
	    ch0->RBCTR=
		bitfield<USIC_CH_RBCTR_DPTR_Msk>(base)
		| bitfield<USIC_CH_RBCTR_LIMIT_Msk>(0)
		| bitfield<USIC_CH_RBCTR_SIZE_Msk>(pr_size);
	    base+=size;
	}
	if(rx1>=size) {
	    ch1->RBCTR=
		bitfield<USIC_CH_RBCTR_DPTR_Msk>(base)
		| bitfield<USIC_CH_RBCTR_LIMIT_Msk>(0)
		| bitfield<USIC_CH_RBCTR_SIZE_Msk>(pr_size);
	    base+=size;
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
// Full duplex serial, no interrupts
////////////////////////////////////////////////////////////////////////////////
template <class TX_PIN, class RX_PIN>
class full_duplex_no_int {
    constexpr static auto channel=location(dout0(TX_PIN{}));
    constexpr static int oversampling=16;
    constexpr static int stop_bits=1;
    constexpr static int data_bits=8;
    constexpr static XMC_USIC_CH_PARITY_MODE parity_mode=
	XMC_USIC_CH_PARITY_MODE_NONE;
public:
    static constexpr int UNIT=dout0(TX_PIN{}).unit;
    static constexpr int CHANNEL=dout0(TX_PIN{}).channel;

    full_duplex_no_int(TX_PIN t, RX_PIN r) {
	static_assert(
	    uint32_t(location(dout0(TX_PIN{})))
	    ==uint32_t(location(dx0(RX_PIN{}))),
	    "TXD and RXD not on the same channel");
    }

    XMC_USIC_CH_t* operator->() const { return channel; }

    inline void SetBaudrate(int rate)
    {
	// XMC_UART_CH_Init causes the binary to be too large
	uint32_t r=oversampling*rate/100;
	uint32_t f=frequency/100;

	uint32_t rem_min=1024;
	uint32_t pdiv_opt=0, step_opt=0;
	for(uint32_t step=1; step<1025; step++) {
	    uint32_t pdiv=f*step/r; // pdiv*1024
	    if((pdiv&0x3ff) < rem_min && pdiv>1024) {
		pdiv_opt=pdiv>>10;
		step_opt=step;
		rem_min=pdiv&0x3ff;

	    }
	}
	channel->FDR=XMC_USIC_CH_BRG_CLOCK_DIVIDER_MODE_FRACTIONAL
	    | bitfield<USIC_CH_FDR_STEP_Msk>(step_opt-1);
	channel->BRG=
	    bitfield<USIC_CH_BRG_PCTQ_Msk>(0)
	    | bitfield<USIC_CH_BRG_PDIV_Msk>(pdiv_opt-1)
	    | bitfield<USIC_CH_BRG_DCTQ_Msk>(oversampling-1);
    }

    void init(int baud) {
	TX_PIN{}.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |dout0(TX_PIN{}).gpio_mode);
	TX_PIN{}.set(XMC_GPIO_HWCTRL_DISABLED);
	RX_PIN{}.set(XMC_GPIO_MODE_INPUT_TRISTATE);
#if 0
	/* Unfortunately using XMC_UART_CH_Init would not fit */
	XMC_UART_CH_CONFIG_t config{
	  .baudrate=baud,
	  .data_bits=8,
	  .stop_bits=1,
	  .parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
	};
	XMC_UART_CH_Init(channel,&config);
#else
	if constexpr (channel==USIC0_CH0_BASE || channel==USIC0_CH1_BASE)  {
	    UngateClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	    #if defined(PERIPHERAL_RESET_SUPPORTED)
		XMC_SCU_RESET_DeassertPeripheralReset(
		    XMC_SCU_PERIPHERAL_RESET_USIC0);
		while (XMC_SCU_RESET_IsPeripheralResetAsserted(
		    XMC_SCU_PERIPHERAL_RESET_USIC0)
		) {
		}
	    #endif
	}

	#if defined(USIC1)
	if constexpr (channel==USIC1_CH0_BASE || channel==USIC1_CH1_BASE)  {
	    UngateClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	    #if defined(PERIPHERAL_RESET_SUPPORTED)
		XMC_SCU_RESET_DeassertPeripheralReset(
		    XMC_SCU_PERIPHERAL_RESET_USIC1);
		while(XMC_SCU_RESET_IsPeripheralResetAsserted(
		    XMC_SCU_PERIPHERAL_RESET_USIC1)
		) {
		}
	    #endif
	}
	#endif

	#if defined(USIC2)
	if constexpr (channel==USIC2_CH0_BASE || channel==USIC2_CH1_BASE)  {
	    UngateClock(XMC_SCU_PERIPHERAL_CLOCK_USIC2);
	    #if defined(PERIPHERAL_RESET_SUPPORTED)
		XMC_SCU_RESET_DeassertPeripheralReset(
		    XMC_SCU_PERIPHERAL_RESET_USIC2);
		while(XMC_SCU_RESET_IsPeripheralResetAsserted(
		    XMC_SCU_PERIPHERAL_RESET_USIC2)
		) {
		}
	    #endif
	}
	#endif

	channel->KSCFG = (USIC_CH_KSCFG_MODEN_Msk | USIC_CH_KSCFG_BPMODEN_Msk);
	while ((channel->KSCFG & USIC_CH_KSCFG_MODEN_Msk) == 0U)
	    ;

	/* Set USIC channel in IDLE mode */
	channel->CCR &= (uint32_t)~USIC_CH_CCR_MODE_Msk;

	SetBaudrate(baud);

	channel->PCR_ASCMode =
	    bitfield<USIC_CH_PCR_ASCMode_STPB_Msk>(stop_bits-1)
	    | bitfield<USIC_CH_PCR_ASCMode_SP_Msk>(oversampling/2+1)
	    | USIC_CH_PCR_ASCMode_SMD_Msk
	    | USIC_CH_PCR_ASCMode_RSTEN_Msk
	    | USIC_CH_PCR_ASCMode_TSTEN_Msk;
	channel->SCTR= bitfield<USIC_CH_SCTR_WLE_Msk>(data_bits-1)
	    | bitfield<USIC_CH_SCTR_FLE_Msk>(data_bits-1)
	    | bitfield<USIC_CH_SCTR_TRM_Msk>(1)
	    | USIC_CH_SCTR_PDL_Msk;
	channel->TCSR=bitfield<USIC_CH_TCSR_TDEN_Msk>(1)
	    | USIC_CH_TCSR_TDSSM_Msk;
	channel->PSCR=0xFFFFFFFFUL;
	channel->CCR=parity_mode;
#endif

	XMC_UART_CH_SetInputSource(channel,
	    XMC_UART_CH_INPUT_RXD,dx<0>(RX_PIN{}));
	//XMC_UART_CH_EnableEvent(channel, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
	XMC_UART_CH_Start(channel);
    }
    void disable(void) {
	channel->CCR=0;
    }

    void tx(uint8_t d) {
	while(channel->TCSR & XMC_USIC_CH_TBUF_STATUS_BUSY)
	    ;
	channel->PSCR=USIC_CH_PSR_ASCMode_TBIF_Msk;
	channel->TBUF[0]=d;
    }

    template <class T>
    void tx_fifo(T const &d) {
       auto p=reinterpret_cast<uint8_t const*>(&d);
       for(int c=0; c<sizeof(T); c++)
           channel->IN[0]=uint32_t(*p++);
    }

    /* Not to be called from interrupt handlers. */
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

    int rx(void)
    {
	if(channel->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk|USIC_CH_RBUFSR_RDV1_Msk))
	    return channel->RBUF;
	return -1;
    }

    template <int num>
    void enable_rx_interrupt(void) {
	auto x=channel->INPR;
	x&=~USIC_CH_INPR_RINP_Msk;
	x|=bitfield<USIC_CH_INPR_RINP_Msk>(num);
	channel->INPR=x;
	channel->CCR|=USIC_CH_CCR_RIEN_Msk;
	/* PCR ASC, xmc4400 15-66. not used */
    }

    void disable_rx_interrupt(int num) {
	channel->CCR&=~USIC_CH_CCR_RIEN_Msk;
    }

    template <int i>
    constexpr IRQn_Type irq(void) {
	constexpr auto unit=dout0(TX_PIN{}).unit;
	if constexpr (unit==0) {
	    return IRQn_Type(USIC0_0_IRQn+i);
	} else if constexpr (unit==1) {
	    return IRQn_Type(USIC0_0_IRQn+6+i); // FIXME, always true?
	}
    }

};

}
