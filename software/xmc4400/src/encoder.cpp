/* AKM63 a.angle_offset=2*pi*20/360
*/
#include <atomic>
#include <math.h>
#include <array>

#include "encoder.h"
#include "uart.h"
#include "crc_lut.h"

uart::full_duplex fd(ENC_TXD,ENC_RXD);
uart::half_duplex hd(ENC_TXD);

constexpr auto PI=acos(-1);

decltype(glass_scale) glass_scale;

typedef std::chrono::duration<int,std::ratio<1,int(1s/pwm_time/4)>> timebase_t;

/*******************************************************************************
    Dummy encoder, encoder is initially of this type and it does nothing
    and is never valid.
*******************************************************************************/
class dummy_encoder_t:public encoder_t {
public:
    dummy_encoder_t();

    void trigger() override {}
    void rx_handler() override {}
    void tx_handler() override {}
    void protocol_handler() override {}
};

dummy_encoder_t::dummy_encoder_t()
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
    mitsubishi_MFS13_t();
    ~mitsubishi_MFS13_t() override;

    void trigger() override;
    void rx_handler() override;
    void tx_handler() override;
    void protocol_handler() override {}
};

mitsubishi_MFS13_t::mitsubishi_MFS13_t()
{
    set_angle_conversion(increments_per_revolution-1,conv);
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

mitsubishi_MFS13_t::~mitsubishi_MFS13_t()
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
}

void mitsubishi_MFS13_t::trigger()
{
    ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |uart::dout0(ENC_TXD).gpio_mode);
    ENC_DIR=1;
    hd->TBUF[0]=0x1a;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    crc=0x1a;
    trigger_count++;
}

void mitsubishi_MFS13_t::tx_handler() {
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TSIF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TSIF_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
}

void mitsubishi_MFS13_t::rx_handler() {
    if(hd->TRBSR & USIC_CH_TRBSCR_CSRBI_Msk) {
	trigger_count--;
	hd->TRBSCR=USIC_CH_TRBSCR_CSRBI_Msk;
	int d;
	while((d=hd.rx_fifo())>=0) {
	    rx_buffer[putp++]=d;
	    crc^=d;
	}
	if(putp==10 && crc==0) {
	    int32_t p=
		rx_buffer[3]
		+(1<<8)*rx_buffer[4]
		+(1<<16)*rx_buffer[5]	// only 4 lsb
		+(1<<20)*rx_buffer[6]
		+(1<<28)*(rx_buffer[7]&0x0f);
	    new_position(p);
	} else if(putp>=10)
	    invalidate();
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
    mitsubishi_PQ_t();
    ~mitsubishi_PQ_t() override;

    void trigger() override;
    void rx_handler()  override;
    void tx_handler()  override {}
    void protocol_handler() override {}
};

mitsubishi_PQ_t::mitsubishi_PQ_t()
{
    set_angle_conversion(increments_per_revolution-1,conv);
    ENC_DIR=1;
    ENC_5V=1;
    fd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    fd.enable_receive_buffer_interrupt<rx_irq>(8);
    NVIC_SetPriority(fd.irq<rx_irq>(), 20);
    NVIC_EnableIRQ(fd.irq<rx_irq>());
}

mitsubishi_PQ_t::~mitsubishi_PQ_t()
{
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(fd.irq<p_irq>());
    fd.disable();
}

void mitsubishi_PQ_t::trigger()
{
    fd->TBUF[0]=0x1a;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    crc=0;
    trigger_count++;
}

void mitsubishi_PQ_t::rx_handler() {
    if(fd->TRBSR & USIC_CH_TRBSCR_CSRBI_Msk) {
	trigger_count--;
	fd->TRBSCR=USIC_CH_TRBSCR_CSRBI_Msk;
	int d;
	while((d=fd.rx_fifo())>=0) {
	    rx_buffer[putp++]=d;
	    crc^=d;
	}
	if(putp==9 && crc==0) {
	    int32_t p=((rx_buffer[2]+(1<<8)*rx_buffer[3] +(1<<12)*rx_buffer[5]
		+(1<<20)*rx_buffer[6]+(1<<28)*rx_buffer[7])<<4)>>4;
	    new_position(p);
	} else if(putp>=9)
	    invalidate();
    }
}


/* AMT21 encoder **************************************************************/
class AMT21_t:public encoder_t
{
    constexpr static int increments_per_revolution=(1<<14);
    constexpr static auto baudrate=uart::Baudrate(2.0e6);
    constexpr static int subsample=2; // The encoder can't keep up

    int putp;
    std::array<uint8_t,8> rx_buffer;
    int subsample_counter;
    int32_t prev_pos;

public:
    AMT21_t(int);
    ~AMT21_t() override;

    void trigger() override;
    void rx_handler()  override;
    void tx_handler()  override;
    void protocol_handler() override {}
};

AMT21_t::AMT21_t(int poles)
{
    float conv=2.0*PI*poles/increments_per_revolution;
    set_angle_conversion(increments_per_revolution-1,conv);

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

AMT21_t::~AMT21_t()
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
}

void AMT21_t::trigger()
{
    if(++subsample_counter<subsample)
	return;
    subsample_counter=0;
    ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |uart::dout0(ENC_TXD).gpio_mode);
    ENC_DIR=1;
    hd->TBUF[0]=0x54;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    trigger_count++;
}

void AMT21_t::tx_handler() {
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TSIF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TSIF_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
}

void AMT21_t::rx_handler() {
    int d;
    while((d=hd.rx_fifo())>=0 && putp<rx_buffer.size()) {
	rx_buffer[putp++]=d;
	if(putp==3 && rx_buffer[0]==0x54) {
	    trigger_count--;
	    uint16_t pos=(uint16_t(rx_buffer[2])<<8) | rx_buffer[1];
	    auto check=pos;
	    for(int i=0;i<7;i++) {
		auto o=check;
		check>>=2;
		check^=o&3;
	    }
	    if(check==3) {
		int32_t np=(pos&0x3fff)|(prev_pos&0xffffc000);
		int32_t dp=np-prev_pos;
		if(dp>8192)
		    np-=0x4000;
		else if(dp<-8192)
		    np+=0x4000;
		prev_pos=np;
		new_position(np);
	    } else
		invalidate();
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
    uint8_t rx_buffer[16];
    uint8_t tx_buffer[8];
    uint8_t addr=0x40;
    timebase_t powerup_delay=
	std::chrono::duration_cast<timebase_t>(20.0ms);

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

    hiperface_t();
    ~hiperface_t() override;

    void trigger() override;
    void rx_handler()  override {}
    void tx_handler()  override;
    void protocol_handler() override {}
};

hiperface_t::hiperface_t()
{
    using namespace std::chrono_literals;

    set_angle_conversion(increments_per_revolution-1,conv);

    static_assert(baudrate.pdiv>0 && baudrate.pdiv<=1024, "PDIV out of range");
    static_assert(baudrate.step>0 && baudrate.step<=1024, "STEP out of range");
    static_assert(baudrate.dcqt>0 && baudrate.dcqt<=32, "DCQT out of range");

    SUPPLY_VOLTAGE=0;

    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    hd.init(baudrate,XMC_USIC_CH_PARITY_MODE_EVEN);
    uart::fifo_configure<0,16>(hd);
    posif_t::init();

    hd.enable_transmit_shift_interrupt<tx_irq>();
    NVIC_SetPriority(hd.irq<tx_irq>(), 0);
    NVIC_EnableIRQ(hd.irq<tx_irq>());

    // Powerup and wait
    poll_timer=poll_interval;
}

hiperface_t::~hiperface_t()
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_12V=0;
    NVIC_DisableIRQ(hd.irq<tx_irq>());
    NVIC_DisableIRQ(hd.irq<rx_irq>());
    hd.disable();
    SUPPLY_VOLTAGE=1;
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

void hiperface_t::trigger()
{
    IO0=0;
    if(powerup_delay>0ms && --powerup_delay==0ms)
	ENC_12V=1;
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
    if(state==DONE) {
	new_position(count());
    } else
	invalidate();
}

class incremental_encoder_t:public encoder_t,
    public posif::qdi32_t<decltype(ENC_SIN),decltype(ENC_COS),
	decltype(ENC_CLK)>
{
    /* AKM63B encoder requires 120 Ohm resistor on index
    */
    constexpr static int poles=5;
    constexpr static int increments_per_revolution=8192;
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;

    using base_t=
	posif::qdi32_t<decltype(ENC_SIN),decltype(ENC_COS),decltype(ENC_CLK)>;
public:
    incremental_encoder_t() {
	set_angle_conversion(0xffffffff,conv);
	init();
	ENC_5V=1;
	ENC_12V=0;
	ENC_DIR=0;
	ENC_CLK.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	ENC_RXD.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	// ERU0_ETL0_INPUTA_P0_1
	skip_posif_index();
    }
    void trigger() override { new_position(count(),base_t::index()); }
    void rx_handler() override {}
    void tx_handler() override {}
    void protocol_handler() override {}
};


/* Fanuc***********************************************************/
   Datarate is 1024kb/s. Send a positive pulse on REQ (pin-5) of 7..9us
   to get a response.  The response is 4 frames of 1 start bit, 16 data
   bits and 1 stop bit.  After the 4th frame the CRC is transmitted,
   without a preceding startbit.  The way to capture it, is to change
   the word length of the 4th frame to 16+1+5 bits.

    1st status
	0xe000	3-msb changes, reason unknown
	0x0100	index seen
    2nd 16-bit counter within revolution
    3rd revolution counter
    4th absolute
	0x8000	Always set?
	0x03fc	Commutation angle? After index the same as 2nd>>4
    5th	5-bit CRC
*/
class fanuc_beta32b:public encoder_t
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<16);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
    constexpr static uint16_t request=0xff80;

    constexpr static uint8_t crc_generator=0xb;
public:

    constexpr static auto baudrate=uart::Baudrate(1.024e6);
    uint16_t rx_buffer[5];
    int putp;
    int pattern[8];
public:
    fanuc_beta32b();
    ~fanuc_beta32b() override;

    void trigger() override;
    void rx_handler()  override;
    void tx_handler()  override {}
    void protocol_handler() override {}
};

fanuc_beta32b::fanuc_beta32b()
{
    set_angle_conversion(increments_per_revolution-1,conv);
    ENC_DIR=1;
    ENC_5V=1;
    fd.init(baudrate,XMC_USIC_CH_PARITY_MODE_NONE,16);
    uart::fifo_configure<0,8>(hd);

    NVIC_SetPriority(fd.irq<rx_irq>(), 0);
    NVIC_EnableIRQ(fd.irq<rx_irq>());
}

fanuc_beta32b::~fanuc_beta32b()
{
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(fd.irq<p_irq>());
    fd.disable();
}

void fanuc_beta32b::trigger()
{
    fd.enable_receive_buffer_interrupt<rx_irq>(2);
    fd.frame_length(15);
    fd->TBUF[0]=request;
    hd->TRBSCR=USIC_CH_TRBSCR_FLUSHRB_Msk;
    putp=0;
    trigger_count++;
    itm.PORT[0].u32=trigger_count;
}

void fanuc_beta32b::rx_handler() {
    if(fd->TRBSR & USIC_CH_TRBSCR_CSRBI_Msk) {
	fd.frame_length(21);
	fd.enable_receive_buffer_interrupt<rx_irq>(1);
	fd->TRBSCR=USIC_CH_TRBSCR_CSRBI_Msk;
	int32_t d;
	while((d=fd.rx_fifo())>=0) {
	    itm.PORT[putp].u16=d;
	    itm.PORT[1].u8=putp;
	    rx_buffer[putp++]=d;
	}
	if(putp<5)
	    return;
	crc_lut_t<5,crc_generator,uint8_t> crc;
	for(int i=0; i<4; i++) {
	    crc.add_bit(0);
	    crc.add_byte(rx_buffer[i]);
	    crc.add_byte(rx_buffer[i]>>8);
	    crc.add_bit(1);
	}
	for(int i=1; i<6; i++)
	    crc.add_bit(rx_buffer[4]>>i);
	itm.PORT[3].u8=crc;
	if(crc!=0)
	    invalidate();
	else if(rx_buffer[0]&0x100) {
	    trigger_count--;
	    itm.PORT[0].u32=trigger_count;
	    // index not yet seen
	    new_position((rx_buffer[3]&0xfff)<<4,0);
	} else {
	    trigger_count--;
	    itm.PORT[0].u32=trigger_count;
	    uint32_t p=rx_buffer[1]+(((uint32_t)rx_buffer[2])<<16);
	    new_position(p,p&0xffff0000);
	}

    }
}


////////////////////////////////////////////////////////////////////////////////
void init_encoder()
{
    static_assert(glass_scale.UNIT!=hiperface_t::UNIT, "Posif overlap");
    /* 2.2kW AB motor,
	angle_offset=0.88
	P=-0.2;
	I=-5e-3;
	encoder.set<AMT21_t>();
    */
    //encoder.set<mitsubishi_PQ_t>();
    //encoder.set<mitsubishi_MFS13_t>();
    //encoder.set<incremental_encoder_t>();
    //encoder.set<hiperface_t>();
    encoder.set<fanuc_beta32b>();
    glass_scale.init();
    glass_scale.skip_posif_index();
    ccu4::slice_t<1,0> h;
    ccu4::slice_t<1,1> l;
    ccu4::start(h,l);
}

void set_encoder(int type, int poles)
{
    type&=0xff;
    switch(type) {
    case 1: encoder.set<mitsubishi_PQ_t>(); break;
    case 2: encoder.set<mitsubishi_MFS13_t>(); break;
    case 3: encoder.set<AMT21_t>(poles); break;
    //case 4: encoder.set<hiperface_t>(poles); break;
    case 5: encoder.set<fanuc_beta32b>(); break;
    }
}

/*******************************************************************************
    Interrupt handlers
*******************************************************************************/

/* Transmit buffer event */
extern "C" void USIC0_0_IRQHandler()
{
    static_assert(encoder_t::tx_irq==0, "Transmit should be mapped to IRQ0");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->tx_handler();
}

/* Receive buffer event */
extern "C" void USIC0_1_IRQHandler()
{
    static_assert(encoder_t::rx_irq==1, "Receive should be mapped to IRQ1");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->rx_handler();
}

/* Protocol event */
extern "C" void USIC0_2_IRQHandler()
{
    static_assert(encoder_t::p_irq==2, "Protocol should be mapped to IRQ2");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->protocol_handler();
}
