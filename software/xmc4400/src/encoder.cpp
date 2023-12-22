#include <atomic>
#include <math.h>
#include <array>

#include "encoder.h"
#include "uart.h"


uart::full_duplex fd(ENC_TXD,ENC_RXD);
uart::half_duplex hd(ENC_TXD);

constexpr auto PI=acos(-1);

decltype(glass_scale) glass_scale;
extern posif::qdi32_t<decltype(ENC_A),decltype(ENC_B),decltype(ENC_Z)>
    glass_scale;

typedef std::chrono::duration<int,std::ratio<4,int(1s/pwm_time)>> timebase_t;

/*******************************************************************************
    Dummy encoder, encoder is initially of this type and it does nothing
    and is never valid.
*******************************************************************************/
class dummy_encoder_t:public encoder_t {
public:
    dummy_encoder_t(void);

    virtual void trigger(void) override {}
    virtual void rx_handler(void) override {}
    virtual void tx_handler(void) override {}
    virtual void protocol_handler(void) override {}
};

dummy_encoder_t::dummy_encoder_t(void)
{
    // Turn encoder power off
    ENC_5V=0;
    ENC_12V=0;
    ENC_DIR=0;
}

decltype(encoder) encoder=decltype(encoder)::make<dummy_encoder_t>();

/* HC-MFS13-S13 motor *********************************************************/
class mitsubishi_MFS13_t:public encoder_t {
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<20);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;

    constexpr static auto baudrate=uart::Baudrate(2.5e6);
    int putp;
    uint8_t rx_buffer[16];
    uint8_t crc;
public:
    mitsubishi_MFS13_t(void);
    virtual ~mitsubishi_MFS13_t(void) override;

    virtual void trigger(void) override;
    virtual void rx_handler(void) override;
    virtual void tx_handler(void) override;
    virtual void protocol_handler(void) override {}
};

mitsubishi_MFS13_t::mitsubishi_MFS13_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=1;
    hd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    hd.enable_transmit_shift_interrupt<tx_irq>();
    NVIC_SetPriority(hd.irq<tx_irq>(), 0);
    NVIC_EnableIRQ(hd.irq<tx_irq>());

    hd.enable_receive_buffer_interrupt<rx_irq>(9);
    NVIC_SetPriority(hd.irq<rx_irq>(), 20);
    NVIC_EnableIRQ(hd.irq<rx_irq>());
}

mitsubishi_MFS13_t::~mitsubishi_MFS13_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
}

void mitsubishi_MFS13_t::trigger(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |uart::dout0(ENC_TXD).gpio_mode);
    ENC_DIR=1;
    hd->TBUF[0]=0x1a;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    crc=0x1a;
}

void mitsubishi_MFS13_t::tx_handler(void) {
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TSIF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TSIF_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
}

void mitsubishi_MFS13_t::rx_handler(void) {
    if(hd->TRBSR & USIC_CH_TRBSCR_CSRBI_Msk) {
	hd->TRBSCR=USIC_CH_TRBSCR_CSRBI_Msk;
	int d;
	while((d=hd.rx_fifo())>=0) {
	    rx_buffer[putp++]=d;
	    crc^=d;
	}
	if(putp==10 && crc==0) {
	    position=
		rx_buffer[3]
		+(1<<8)*rx_buffer[4]
		+(1<<16)*rx_buffer[5]	// only 4 lsb
		+(1<<20)*rx_buffer[6]
		+(1<<28)*(rx_buffer[7]&0x0f);
	    angle=conv*float(
		rx_buffer[3]+(1<<8)*rx_buffer[4]+(1<<16)*(rx_buffer[5]));
	    valid=1;
	} else if(putp>=10)
	    valid=0;
    }
}

/* HC-PQ[24]3 motor ***********************************************************/
class mitsubishi_PQ_t:public encoder_t
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;

    constexpr static auto baudrate=uart::Baudrate(2.5e6);
    int putp;
    uint8_t rx_buffer[16];
    uint8_t crc;
public:
    mitsubishi_PQ_t(void);
    virtual ~mitsubishi_PQ_t(void);

    virtual void trigger(void) override;
    virtual void rx_handler(void)  override;
    virtual void tx_handler(void)  override {}
    virtual void protocol_handler(void) override {}
};

mitsubishi_PQ_t::mitsubishi_PQ_t(void)
{
    ENC_DIR=1;
    ENC_5V=1;
    fd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    fd.enable_receive_buffer_interrupt<rx_irq>(8);
    NVIC_SetPriority(fd.irq<rx_irq>(), 20);
    NVIC_EnableIRQ(fd.irq<rx_irq>());
}

mitsubishi_PQ_t::~mitsubishi_PQ_t(void)
{
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(fd.irq<p_irq>());
    fd.disable();
}

void mitsubishi_PQ_t::trigger(void)
{
    fd->TBUF[0]=0x1a;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    crc=0;
}

void mitsubishi_PQ_t::rx_handler(void) {
    if(fd->TRBSR & USIC_CH_TRBSCR_CSRBI_Msk) {
	fd->TRBSCR=USIC_CH_TRBSCR_CSRBI_Msk;
	int d;
	while((d=fd.rx_fifo())>=0) {
	    rx_buffer[putp++]=d;
	    crc^=d;
	}
	if(putp==9 && crc==0) {
	    position=((rx_buffer[2]+(1<<8)*rx_buffer[3] +(1<<12)*rx_buffer[5]
		+(1<<20)*rx_buffer[6]+(1<<28)*rx_buffer[7])<<4)>>4;
	    angle=conv*float(rx_buffer[2]+(1<<8)*rx_buffer[3]);
	    valid=1;
	} else if(putp>=9)
	    valid=0;
    }
}


/* AMT21 encoder **************************************************************/
class AMT21_t:public encoder_t
{
    constexpr static int increments_per_revolution=(1<<14);
    constexpr static auto baudrate=uart::Baudrate(2.0e6);

    float conv;//=2.0*PI*poles/increments_per_revolution;
    int putp;
    std::array<uint8_t,8> rx_buffer;
public:
    AMT21_t(int);
    virtual ~AMT21_t(void);

    virtual void trigger(void) override;
    virtual void rx_handler(void)  override;
    virtual void tx_handler(void)  override;
    virtual void protocol_handler(void) override {}
};

AMT21_t::AMT21_t(int poles)
{
    conv=2.0*PI*poles/increments_per_revolution;

    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=1;
    hd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    hd.enable_transmit_shift_interrupt<tx_irq>();
    NVIC_SetPriority(hd.irq<tx_irq>(), 0);
    NVIC_EnableIRQ(hd.irq<tx_irq>());

    hd.enable_rx_interrupt<rx_irq>();
    NVIC_SetPriority(hd.irq<rx_irq>(), 20);
    NVIC_EnableIRQ(hd.irq<rx_irq>());
}

AMT21_t::~AMT21_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
}

void AMT21_t::trigger(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |uart::dout0(ENC_TXD).gpio_mode);
    ENC_DIR=1;
    hd->TBUF[0]=0x54;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
}

void AMT21_t::tx_handler(void) {
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TSIF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TSIF_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
}

void AMT21_t::rx_handler(void) {
    int d;
    while((d=hd.rx_fifo())>=0 && putp<rx_buffer.size()) {
	rx_buffer[putp++]=d;
	if(putp==3 && rx_buffer[0]==0x54) {
	    uint16_t pos=(uint16_t(rx_buffer[2])<<8) | rx_buffer[1];
	    auto check=pos;
	    for(int i=0;i<7;i++) {
		auto o=check;
		check>>=2;
		check^=o&3;
	    }
	    if(check==3) {
		int32_t np=(pos&0x3fff)|(position&0xffffc000);
		int32_t dp=np-position;
		if(dp>8192)
		    np-=0x4000;
		else if(dp<-8192)
		    np+=0x4000;

		position=np;
		angle=conv*float(position&0x3fff);
		valid=1;
	    } else
		valid=0;
	}
    }
}

/*******************************************************************************
    Hiperface encoder
*******************************************************************************/

class hiperface_t:public encoder_t,
    public posif::qd32_t<decltype(ENC_SIN),decltype(ENC_COS)>
{
    using posif_t=posif::qd32_t<decltype(ENC_SIN),decltype(ENC_COS)>;
#if 0
    // FIXME, these settings are for the DS56S
    constexpr static int poles=3;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
    constexpr static float offset=PI/3;
#else
    // Allen Bradley (P2,P1,P0=1,3,2)
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
    constexpr static float offset=PI/3;
#endif

    constexpr static auto baudrate=uart::Baudrate(9600);
    constexpr static timebase_t poll_interval=
	std::chrono::duration_cast<timebase_t>(200.0ms);

    timebase_t poll_timer;
    enum state_t { STARTUP, STATUS, POSITION, DONE } state;
    uint32_t status;
    int tx_get, tx_len, rx_put;

    void transmit(std::initializer_list<uint8_t> msg)
    {
	uint8_t crc=0;
	tx_len=0;
	for(auto x: msg) {
	    crc^=x;
	    tx_buffer[tx_len++]=x;
	}
	tx_buffer[tx_len++]=crc;
	tx_get=0;
	rx_put=0;

	ENC_DIR=1;
	ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL
	    | uart::dout0(ENC_TXD).gpio_mode);
	hd.tx(tx_buffer[tx_get++]);
    }

public:
    using qd32_t<decltype(ENC_SIN),decltype(ENC_COS)>::UNIT;

    hiperface_t(void);
    virtual ~hiperface_t(void);

    virtual void trigger(void) override;
    virtual void rx_handler(void)  override {}
    virtual void tx_handler(void)  override;
    virtual void protocol_handler(void) override {}
};

hiperface_t::hiperface_t(void)
{
    using namespace std::chrono_literals;

    static_assert(baudrate.pdiv>0 && baudrate.pdiv<=1024, "PDIV out of range");
    static_assert(baudrate.step>0 && baudrate.step<=1024, "STEP out of range");
    static_assert(baudrate.dcqt>0 && baudrate.dcqt<=32, "DCQT out of range");

    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_12V=1;
    hd.init(baudrate,XMC_USIC_CH_PARITY_MODE_EVEN);
    uart::fifo_configure<0,16>(hd);
    posif_t::init();

    hd.enable_transmit_shift_interrupt<tx_irq>();
    NVIC_SetPriority(hd.irq<tx_irq>(), 0);
    NVIC_EnableIRQ(hd.irq<tx_irq>());

    // Powerup and wait
    ENC_12V=1;
    poll_timer=poll_interval;
}

hiperface_t::~hiperface_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_12V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
}

void hiperface_t::tx_handler()
{
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TSIF_Msk) {
	if(tx_get<tx_len) {
	    hd.tx(tx_buffer[tx_get++]);
	} else {
	    ENC_DIR=0;
	    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
	}
	hd->PSCR=USIC_CH_PSR_ASCMode_TSIF_Msk;
    }
}

void hiperface_t::trigger(void)
{
    IO0=0;
    if(poll_timer>0ms)
	poll_timer--;
    else {
	uint8_t crc=0;
	int d;
	while((d=hd.rx_fifo())>=0) {
	    rx_buffer[rx_put++]=d;
	    crc^=d;
	}

	if(state==STARTUP)
	    state=STATUS;
	else if(rx_buffer[4]==0x50) {
	    status=rx_buffer[5];
	    if(!status && !crc)
		state=POSITION;
	    else
		state=STATUS;
	} else if(state==POSITION && rx_buffer[4]==0x42 && rx_put==10 && !crc) {
	    state=DONE;
	    IO0=1;
	    uint32_t position=rx_buffer[8]
		+0x100L*rx_buffer[7]
		+0x10000L*rx_buffer[6]
		+0x1000000L*rx_buffer[5];
	    setcount(position>>3);
	} else if(state!=DONE)
	    state=STARTUP;

	poll_timer=poll_interval;
	switch(state) {
	case STARTUP:
	    transmit({addr,0x53});
	    break;
	case STATUS:
	    transmit({addr,0x50});
	    break;
	case POSITION:
	    transmit({addr,0x42});
	    break;
	}
    }
    position=count();
    constexpr float F=2.0*std::numbers::pi/1024.0f;
    angle=float(position&0x3ff)*F;
    valid=state==DONE;
}

////////////////////////////////////////////////////////////////////////////////
void init_encoder(void)
{
    static_assert(glass_scale.UNIT!=hiperface_t::UNIT, "Posif overlap");
    /* 2.2kW AB motor,
	angle_offset=0.88
	P=-0.2;
	I=-5e-3;
	encoder.set<AMT21_t>();
    */
    //encoder.set<mitsubishi_PQ_t>();
    encoder.set<mitsubishi_MFS13_t>();
    glass_scale.init();
    ccu4::slice_t<1,0> h;
    ccu4::slice_t<1,1> l;
    ccu4::start(h,l);
}

void set_encoder(int type, int poles)
{
    switch(type) {
    case 1: encoder.set<mitsubishi_PQ_t>(); break;
    case 2: encoder.set<mitsubishi_MFS13_t>(); break;
    case 3: encoder.set<AMT21_t>(poles); break;
    //case 4: encoder.set<hiperface_t>(poles); break;
    }
}

/*******************************************************************************
    Interrupt handlers
*******************************************************************************/

/* Transmit buffer event */
extern "C" void USIC0_0_IRQHandler(void)
{
    static_assert(encoder_t::tx_irq==0, "Transmit should be mapped to IRQ0");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->tx_handler();
}

/* Receive buffer event */
extern "C" void USIC0_1_IRQHandler(void)
{
    static_assert(encoder_t::rx_irq==1, "Receive should be mapped to IRQ1");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->rx_handler();
}

/* Protocol event */
extern "C" void USIC0_2_IRQHandler(void)
{
    static_assert(encoder_t::p_irq==2, "Protocol should be mapped to IRQ2");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->protocol_handler();
}
