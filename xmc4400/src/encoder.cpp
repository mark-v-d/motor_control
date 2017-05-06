#include "encoder.h"
#include "hardware.h"

#include "xmc_scu.h"
#include "xmc_ccu4.h"
#include "xmc_eru.h"

#include <atomic>
#include <math.h>

#include "bitfields.h"

constexpr auto PI=acos(-1);

/*******************************************************************************
    Dummy encoder, encoder is initially of this type and it does nothing
    and is never valid.
*******************************************************************************/
class dummy_encoder_t:public encoder_t {
public:
    dummy_encoder_t(void);

    virtual int32_t position(void) { return 0;}
    virtual float angle(void) { return 0.0;}
    virtual bool valid(void) { return false;}

    virtual void trigger(void) {}
    virtual void half_duplex(void) {}
    virtual void full_duplex(void) {}
};

dummy_encoder_t::dummy_encoder_t(void)
{
    // Turn encoder power off
    ENC_5V=0;
    ENC_12V=0;
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_TRISTATE);
    ENC_DIR=0;

    // disable all encoder peripherals and interrupts
    // FIXME, posif and CCU40 missing
    if(usic_ch_ns::channel(ENC_TXD)) {
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	NVIC_DisableIRQ(USIC1_0_IRQn);
	NVIC_DisableIRQ(USIC1_1_IRQn);
    } else {
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC0);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	NVIC_DisableIRQ(USIC0_0_IRQn);
	NVIC_DisableIRQ(USIC0_1_IRQn);
    }
}

std::unique_ptr<encoder_t> encoder=std::make_unique<dummy_encoder_t>();

void encoder_t::init_half_duplex(
    XMC_UART_CH_CONFIG_t const &uart_config)
{
    using namespace usic_ch_ns;
    constexpr XMC_USIC_CH_t *channel=xmc_channel(ENC_TXD);

    XMC_UART_CH_DisableEvent(ENC_TXD, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    NVIC_DisableIRQ(irq<fd_irq>(ENC_TXD));

    XMC_UART_CH_Init(channel, &uart_config);
    XMC_UART_CH_SetInputSource(channel, XMC_UART_CH_INPUT_RXD, dx0(ENC_TXD));
    XMC_UART_CH_EnableEvent(channel, XMC_UART_CH_EVENT_FRAME_FINISHED );
    XMC_UART_CH_SelectInterruptNodePointer(channel,
	XMC_UART_CH_INTERRUPT_NODE_POINTER_PROTOCOL, hd_irq);
    XMC_UART_CH_Start(channel);

    channel->RBCTR=rbctr_t({{
	.dptr=0,
	.limit=0,
	.srbtm=0,
	.srbten=0,
	.srbinp=0,
	.arbinp=0,
	.rcim=0,
	.size=4,	// 16 entry fifo
	.rnm=0,
	.lof=0,
	.arbien=0,
	.srbien=0,
	.rberien=0
    }}).raw;

    // Protocol interrupt
    NVIC_SetPriority(irq<hd_irq>(ENC_TXD),  0);
    NVIC_ClearPendingIRQ(irq<hd_irq>(ENC_TXD));
    NVIC_EnableIRQ(irq<hd_irq>(ENC_TXD));
}

void encoder_t::init_full_duplex(
    XMC_UART_CH_CONFIG_t const &uart_config)
{
    using namespace usic_ch_ns;
    constexpr XMC_USIC_CH_t *channel=xmc_channel(ENC_TXD);

    XMC_UART_CH_DisableEvent(ENC_TXD, XMC_UART_CH_EVENT_FRAME_FINISHED);
    NVIC_DisableIRQ(irq<hd_irq>(ENC_TXD));

    XMC_UART_CH_Init(channel, &uart_config);
    XMC_UART_CH_SetInputSource(channel, XMC_UART_CH_INPUT_RXD, dx0(ENC_RXD));
    XMC_UART_CH_EnableEvent(channel, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    XMC_UART_CH_SelectInterruptNodePointer(channel,
	XMC_UART_CH_INTERRUPT_NODE_POINTER_RECEIVE, fd_irq);
    XMC_UART_CH_EnableInputInversion(channel,XMC_UART_CH_INPUT_RXD);
    XMC_UART_CH_Start(channel);

    channel->RBCTR=rbctr_t({{
	.dptr=0,
	.limit=0,
	.srbtm=0,
	.srbten=0,
	.srbinp=0,
	.arbinp=0,
	.rcim=0,
	.size=4,	// 16 entry fifo
	.rnm=0,
	.lof=0,
	.arbien=0,
	.srbien=0,
	.rberien=0
    }}).raw;

    // Receive interrupt
    NVIC_SetPriority(irq<fd_irq>(ENC_TXD),  0);
    NVIC_ClearPendingIRQ(irq<fd_irq>(ENC_TXD));
    // NVIC_EnableIRQ(irq<fd_irq>(ENC_TXD));
}

/*******************************************************************************
    Base class for using POSIF
*******************************************************************************/
#include "xmc_posif.h"

template <typename enc_a, typename enc_b, typename enc_z>
class posif_t {
protected:
    int32_t timer=0;
public:
    void posif_init(uint32_t position);
    void posif_latch(void);
};

// FIXME, this only works for POSIF0, POSIF1 needs ccu41
template <typename enc_a, typename enc_b, typename enc_z>
void posif_t<enc_a,enc_b,enc_z>::posif_init(
    uint32_t position
) {
    using namespace posif_ns;
    iopin::input<enc_a::PORT,enc_a::PIN> s0;
    iopin::input<enc_b::PORT,enc_b::PIN> s90;
    iopin::input<enc_z::PORT,enc_z::PIN> z;

    static_assert(unit(s90)==unit(s0),
	"s90 and s0 must be part of the same POSIF unit");
    auto p=&posif[unit(s90)];

    XMC_POSIF_CONFIG_t PCONF({{{
	.mode=XMC_POSIF_MODE_QD,
	.input0=pinA(s0),
	.input1=pinB(s90),
	.input2=pinZ(z),
	.filter=7
    }}});
    XMC_POSIF_Init(p, &PCONF);

    XMC_POSIF_QD_CONFIG_t qd;
    qd.phase_a=0;
    qd.phase_b=0;       /**< Phase-B active level configuration */
    qd.phase_leader=0;  /**< Which of the two phase signals[Phase A or Phase B]
			    leads the other? */
    qd.index=XMC_POSIF_QD_INDEX_GENERATION_ALWAYS;
    XMC_POSIF_QD_Init(p, &qd);

    XMC_POSIF_Start(p);

    XMC_ERU_Enable(XMC_ERU1);
    XMC_ERU_ETL_SetInput(XMC_ERU1, pwm.spare_slice(), XMC_ERU_ETL_INPUT_A0, 
	ERU1_ETL0_INPUTB_CCU80_ST0);
    XMC_ERU_ETL_SetSource(XMC_ERU1, pwm.spare_slice(), XMC_ERU_ETL_SOURCE_B);
    XMC_ERU_ETL_SetEdgeDetection(XMC_ERU1, pwm.spare_slice(), 
	XMC_ERU_ETL_EDGE_DETECTION_FALLING);
    XMC_ERU_ETL_SetStatusFlagMode(XMC_ERU1, pwm.spare_slice(), 
	XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL);
    XMC_ERU_ETL_EnableOutputTrigger(XMC_ERU1, pwm.spare_slice(), 
	XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL1);

    XMC_ERU_OGU_DisablePatternDetection(XMC_ERU1, 1);
    XMC_ERU_OGU_DisablePeripheralTrigger(XMC_ERU1, 1);
    XMC_ERU_OGU_SetServiceRequestMode(XMC_ERU1, 1, 
	XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER);

    /* Initialise counter FIXME, allocate unit in some way */
    XMC_CCU4_SetModuleClock(CCU40, XMC_CCU4_CLOCK_SCU);
    XMC_CCU4_EnableModule(CCU40);
    XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    ccu40.GIDLC=0x100;		// pre-scaler out of idle

    // Slice 0 latches the index pulse
    ccu40.cc[0].INS=ccu4_cc4_ns::ins_t({{
	.ev0is=CCU40_IN0_POSIF0_OUT0,	// q_clock
	.ev1is=CCU40_IN0_POSIF0_OUT1,	// q_dir
	.ev2is=CCU40_IN0_POSIF0_OUT3,	// index
	.ev0em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
	.ev1em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_NONE,
	.ev2em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
	.ev0lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
	.ev1lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_COUNT_UP_ON_LOW,
	.ev2lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
	.lpf0m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
	.lpf1m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
	.lpf2m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED
    }}).raw;
    ccu40.cc[0].CMC=ccu4_cc4_ns::cmc_t({{
	.strts=0,	// no external start
	.ends=0,	// no external end
	.cap0s=3,	// index
	.cap1s=0,	// not external capture 1
	.gates=0,	// no external gating
	.uds=2,	// up/down
	.lds=0,	// no external load
	.cnts=1,	// count
	.ofs=0,
	.ts=0,
	.mos=0,
	.tce=0 
    }}).raw; 

    // slice 1 latches at the interrupt
    ccu40.cc[1].INS=ccu4_cc4_ns::ins_t({{
	.ev0is=CCU40_IN1_POSIF0_OUT0,	// q_clock
	.ev1is=CCU40_IN1_POSIF0_OUT1,	// q_dir
	.ev2is=CCU40_IN1_SCU_ERU1_IOUT1, // latch
	.ev0em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
	.ev1em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_NONE,
	.ev2em=XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
	.ev0lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
	.ev1lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_COUNT_UP_ON_LOW,
	.ev2lm=XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH,
	.lpf0m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
	.lpf1m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
	.lpf2m=XMC_CCU4_SLICE_EVENT_FILTER_DISABLED
    }}).raw;
    ccu40.cc[1].CMC=ccu4_cc4_ns::cmc_t({{
	.strts=0,	// no external start
	.ends=0,	// no external end
	.cap0s=3,	// index
	.cap1s=0,	// not external capture 1
	.gates=0,	// no external gating
	.uds=2,	// up/down
	.lds=0,	// no external load
	.cnts=1,	// count
	.ofs=0,
	.ts=0,
	.mos=0,
	.tce=0
    }}).raw; 

    for(int i=0;i<2;i++)
	ccu40.cc[i].PRS=0xffff;	// 16-bit period
    ccu40.GCSS=0x11;		// transfer enable (to load period)
    ccu40.cc[0].TIMER=(position/8)&0xffff;
    ccu40.cc[1].TIMER=(position>>(3+16))&0xffff;
    ccu40.GIDLC=0x3;		// slice[0] out of idle
    for(int i=0;i<2;i++)
	ccu40.cc[i].TCSET=1;	// timer ON
}

template <typename enc_a, typename enc_b, typename enc_z>
inline void posif_t<enc_a,enc_b,enc_z>::posif_latch(void)
{
    timer=ccu40.cc[0].TIMER | (ccu40.cc[1].TIMER<<16);
}

/*******************************************************************************
    Mitsubishi base class
*******************************************************************************/
class mitsubishi_encoder_t:public encoder_t,
    public posif_t<decltype(ENC_A),decltype(ENC_B), decltype(ENC_Z)>
{
protected:
    int putp;
    uint8_t rx_buffer[16];
public:
    virtual int32_t position(void) { return 0; }
    virtual float angle(void) { return 0.0F; }
    virtual bool valid(void) { return false; }

    virtual void trigger(void) { }
    virtual void half_duplex(void);
    virtual void full_duplex(void);

    static int detect(void);
private:
    void serial_tx(uint8_t);
protected:
    void read_fifo(void);
};

inline void mitsubishi_encoder_t::read_fifo(void)
{
    if(!putp) {
	using namespace usic_ch_ns;
	constexpr XMC_USIC_CH_t *channel=xmc_channel(ENC_TXD);
	for(;;) {
	    trbsr_t status={.raw=channel->TRBSR};
	    if(status.rempty) 
		break;
	    if(putp<sizeof(rx_buffer))
		rx_buffer[putp++]=channel->OUTR;
	    else
		int dummy=channel->OUTR;
	}
    }
}

void mitsubishi_encoder_t::half_duplex(void)
{
    using namespace usic_ch_ns;
    ENC_DIR=0;
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_TRISTATE);
    NVIC_DisableIRQ(irq<hd_irq>(ENC_TXD));

    constexpr XMC_USIC_CH_t *channel=xmc_channel(ENC_TXD);
    uint32_t reason=channel->PSR;
    channel->PSCR=reason;
}

void mitsubishi_encoder_t::full_duplex(void)
{
    using namespace usic_ch_ns;
    XMC_USIC_CH_t *channel=xmc_channel(ENC_TXD);
    channel->PSCR=channel->PSR;
    rx_buffer[putp++]=channel->OUTR;
}

void mitsubishi_encoder_t::serial_tx(uint8_t data)
{
    using namespace std::chrono;
    using namespace usic_ch_ns;
    ENC_DIR=1;
    ENC_TXD.set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL
	| XMC_GPIO_MODE_OUTPUT_ALT2));
    xmc_channel(ENC_TXD)->PSCR=0xffffffff;
    NVIC_ClearPendingIRQ(irq<hd_irq>(ENC_TXD));
    NVIC_EnableIRQ(irq<hd_irq>(ENC_TXD));
    putp=0;
    ENC_TXD=data;
    sleep(1ms);
    read_fifo();
}

/* HC-MFS13-S13 motor *********************************************************/
class mitsubishi_MFS13_t:public mitsubishi_encoder_t
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<20);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
public:
    virtual int32_t position(void);
    virtual float angle(void);
    virtual bool valid(void);

    virtual void trigger(void);
};

float mitsubishi_MFS13_t::angle(void)
{
    uint32_t encoder=rx_buffer[3]+(1<<8)*rx_buffer[4]+(1<<16)*rx_buffer[5];
    return conv*float(encoder);
}

int32_t mitsubishi_MFS13_t::position(void)
{
    return
	rx_buffer[3]
	+(1<<8)*rx_buffer[4]
	+(1<<16)*rx_buffer[5]	// only 4 lsb
	+(1<<20)*rx_buffer[6]
	+(1<<28)*(rx_buffer[7]&0x0f);
}

char wrong[16], right[16];
int wrong_length;
bool mitsubishi_MFS13_t::valid(void)
{
    read_fifo();
    if(putp!=10 && putp!=9) {
	for(int i=0;i<putp;i++)
	    wrong[i]=rx_buffer[i];
	wrong_length=putp;
	return false;
    }
    uint8_t crc=0;
    for(int i=putp-9;i<putp;i++)
	crc^=rx_buffer[i];
    if(crc!=0) {
	for(int i=0;i<putp;i++)
	    wrong[i]=rx_buffer[i];
	wrong_length=putp;
	return false;
    } else {
	for(int i=0;i<putp;i++)
	    right[i]=rx_buffer[i];
    }
    return crc==0;
}

void mitsubishi_MFS13_t::trigger(void)
{
    ENC_DIR=1;
    putp=0;
    NVIC_ClearPendingIRQ(usic_ch_ns::irq<hd_irq>(ENC_TXD));
    NVIC_EnableIRQ(usic_ch_ns::irq<hd_irq>(ENC_TXD));
    ENC_TXD.set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL 
	| XMC_GPIO_MODE_OUTPUT_ALT2));
    ENC_TXD=0x1a;
}

/* HC-PQ[24]3 motor ***********************************************************/
class mitsubishi_PQ_t:public mitsubishi_encoder_t
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
public:
    virtual int32_t position(void);
    virtual float angle(void);
    virtual bool valid(void);

    virtual void trigger(void);
};

float mitsubishi_PQ_t::angle(void)
{
    uint32_t encoder=rx_buffer[2]+(1<<8)*rx_buffer[3];
    return conv*float(encoder);
}

int32_t mitsubishi_PQ_t::position(void)
{
    return ((rx_buffer[2]+(1<<8)*rx_buffer[3]
	+(1<<12)*rx_buffer[5]+(1<<20)*rx_buffer[6]+(1<<28)*rx_buffer[7])<<4)>>4;
}

bool mitsubishi_PQ_t::valid(void)
{
    read_fifo();
    if(putp!=9)
	return false;
    uint8_t crc=0;
    for(int i=0;i<putp;i++)
	crc^=rx_buffer[i];
    return crc==0;
}

void mitsubishi_PQ_t::trigger(void)
{
    ENC_DIR=1;
    ENC_TXD.set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL 
	| XMC_GPIO_MODE_OUTPUT_ALT2));
    putp=0;
    ENC_TXD=0x1a;
    posif_latch();
}

/******************************************************************************/
int mitsubishi_encoder_t::detect(void)
{
    using namespace std::chrono;
    encoder=std::make_unique<mitsubishi_encoder_t>();
    auto *p=static_cast<mitsubishi_encoder_t*>(encoder.get());

    XMC_UART_CH_CONFIG_t uart_config = {
	.baudrate=2500000,
	.data_bits=8U,
	.frame_length=0,
	.stop_bits=1,
	.oversampling=0,
	.parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
    };

    init_half_duplex(uart_config);
    NVIC_DisableIRQ(usic_ch_ns::irq<hd_irq>(ENC_TXD));

    // Powerup and wait
    p->serial_tx(0x1a);
    ENC_5V=1;
    sleep(100ms);

    // First try to use half-duplex mode
    p->serial_tx(0x1a);
    if(p->putp==10) {
	p->posif_init(0);
	encoder=std::make_unique<mitsubishi_MFS13_t>();
	return 1;
    }

    init_full_duplex(uart_config);

    p->serial_tx(0x92);
    p->serial_tx(0x92);
    p->serial_tx(0x7a);
    p->serial_tx(0x7a);
    p->serial_tx(0x7a);
    p->serial_tx(0x7a);
    p->serial_tx(0x7a);
    p->serial_tx(0x7a);
    p->serial_tx(0x1a);
    if(p->putp!=9) {
	encoder=std::make_unique<dummy_encoder_t>();
	return 0;
    }

    // p->posif_init(0);
    encoder=std::make_unique<mitsubishi_PQ_t>();
    return 1;
}

/*******************************************************************************
    Hiperface encoder
*******************************************************************************/
class hiperface_t:public encoder_t, 
    public posif_t<decltype(ENC_SIN),decltype(ENC_COS),decltype(ENC_Z)>
{
    // FIXME, these settings are for the DS56S
    constexpr static int poles=3;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
    constexpr static float offset=PI/3;

    uint8_t rx_buffer[8];
    uint8_t tx_buffer[8];
    volatile int rx_put, tx_get, tx_len;
public:
    virtual int32_t position(void);
    virtual float angle(void) { return conv*(timer&0xfff)+offset; }
    virtual bool valid(void) { return true; }

    virtual void trigger(void);
    virtual void half_duplex(void);
    virtual void full_duplex(void) {}

    static int detect(void);
private:
    void transmit(std::initializer_list<uint8_t> msg)
    {
	using namespace std::chrono;
	ENC_DIR=1;
	ENC_TXD.set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | XMC_GPIO_MODE_OUTPUT_ALT2));
	uint8_t crc=0;
	tx_len=0;
	for(auto x: msg) {
	    crc^=x;
	    tx_buffer[tx_len++]=x;
	}
	tx_buffer[tx_len++]=crc;
	tx_get=0;
	rx_put=0;
	ENC_TXD=tx_buffer[tx_get++];
	sleep(20ms);
    }
};

void hiperface_t::trigger(void)
{
    posif_latch();
}

int32_t hiperface_t::position(void) 
{ 
    int32_t ch0=vadc.G[1].RES[0]&0xffff; ch0-=2047;
    int32_t ch1=vadc.G[2].RES[0]&0xffff; ch1-=2047;
    constexpr float c=0.5/PI;
    int32_t ang=1023.0F*(c*atan2f(-ch1,ch0)+0.5F);
    uint32_t t=timer;
    if(ang>>8==3)
	t--;
    if(ang>>8==0)
	t++;
    t&=0xfffffffc;
    return t<<8 | ang;
}


void hiperface_t::half_duplex(void)
{
    XMC_USIC_CH_t *usic=ENC_TXD;
    uint32_t reason=usic->PSR;
    if(reason & USIC_CH_PSR_ASCMode_RFF_Msk) {
	if(ENC_DIR)
	    usic->OUTR;
	else
	    rx_buffer[rx_put++]=usic->OUTR;
    }
    if(reason & USIC_CH_PSR_ASCMode_TFF_Msk) {
	if(tx_get<tx_len)
	    ENC_TXD=tx_buffer[tx_get++];
	else {
	    ENC_TXD.set(XMC_GPIO_MODE_INPUT_TRISTATE);
	    ENC_DIR=0;
	}
    } 
    usic->PSCR=reason;
}

int hiperface_t::detect(void)
{
    using namespace std::chrono;

    encoder=std::make_unique<hiperface_t>();
    auto p=static_cast<hiperface_t*>(encoder.get());

    XMC_UART_CH_CONFIG_t uart_config = {
	.baudrate=9600,
	.data_bits=8U,
	.frame_length=0,
	.stop_bits=1,
	.oversampling=0,
	.parity_mode=XMC_USIC_CH_PARITY_MODE_EVEN
    };

    init_half_duplex(uart_config);

    // Powerup and wait
    ENC_12V=1;
    sleep(200ms);
    p->transmit({0xff, 0x53}); // reset
    do {
	p->transmit({0xff, 0x50});	// ask status
    } while(p->rx_put!=4 || p->rx_buffer[2]);
    p->transmit({0xff,0x42});

    if(p->rx_put!=7) {
	encoder=std::make_unique<dummy_encoder_t>();
	return 0;
    }

    uint32_t position=p->rx_buffer[5]
	+0x100L*p->rx_buffer[4]
	+0x10000L*p->rx_buffer[3]
	+0x1000000L*p->rx_buffer[2];
    p->posif_init(position);

    return 1;
}


void init_encoder(void)
{
    if(mitsubishi_encoder_t::detect())
	return;
    if(hiperface_t::detect())
	return;
}
