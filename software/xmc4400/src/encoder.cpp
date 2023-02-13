#include "encoder.h"
#include "hardware.h"

#include "xmc_scu.h"
#include "xmc_ccu4.h"
#include "xmc_eru.h"
#include "uart.h"

#include <atomic>
#include <math.h>

#include "bitfields.h"
uart::full_duplex fd(ENC_TXD,ENC_RXD);
uart::half_duplex hd(ENC_TXD);

constexpr auto PI=acos(-1);

/*******************************************************************************
    Dummy encoder, encoder is initially of this type and it does nothing
    and is never valid.
*******************************************************************************/
class dummy_encoder_t:public encoder_t {
public:
    dummy_encoder_t(void);

    virtual void trigger(void) override {}
    virtual void rx_handler(void) override {}
    virtual void tb_handler(void) override {}
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

#if 0
/* HC-MFS13-S13 motor *********************************************************/
class mitsubishi_MFS13_t:public encoder_t {
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<20);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;

    constexpr static auto baudrate=uart::Baudrate(2.5e6);
    int putp;
    uint8_t rx_buffer[16];
public:
    mitsubishi_MFS13_t(void);
    virtual ~mitsubishi_MFS13_t(void) override;

    virtual void trigger(void) override;
    virtual void rx_handler(void) override{}
    virtual void tb_handler(void) override{}
    virtual void protocol_handler(void) override;
};

mitsubishi_MFS13_t::mitsubishi_MFS13_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=1;
    hd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    hd.enable_protocol_interrupt<p_irq>();
    NVIC_SetPriority(hd.irq<p_irq>(), 20);
    NVIC_EnableIRQ(hd.irq<p_irq>());
}

mitsubishi_MFS13_t::~mitsubishi_MFS13_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=0;
    NVIC_DisableIRQ(hd.irq<p_irq>());
    hd.disable();
}

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
    ENC_TXD.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL |uart::dout0(ENC_TXD).gpio_mode);
    ENC_DIR=1;
    hd->TBUF[0]=0x1a;
    hd->PSCR=USIC_CH_PSR_ASCMode_TFF_Msk | USIC_CH_PSR_ASCMode_RFF_Msk;
    hd->PCR_ASCMode|=USIC_CH_PCR_ASCMode_FFIEN_Msk;
    putp=0;
}

__attribute__((section(".ram_code")))
void mitsubishi_MFS13_t::protocol_handler(void)
{
    auto x=hd->PSR_ASCMode;
    if(x & USIC_CH_PSR_ASCMode_TFF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TFF_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
    if(x & USIC_CH_PSR_ASCMode_RFF_Msk) {
	rx_buffer[putp++]=hd.rx_fifo();
	itm.PORT[7].u16=rx_buffer[putp-1] | (putp<<8);
	hd->PSCR=USIC_CH_PSR_ASCMode_RFF_Msk;
    }
    NVIC_ClearPendingIRQ(fd.irq<p_irq>());
}
#endif

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
    virtual void rx_handler(void)  override{}
    virtual void tb_handler(void)  override{}
    virtual void protocol_handler(void) override;
};

mitsubishi_PQ_t::mitsubishi_PQ_t(void)
{
    ENC_DIR=1;
    ENC_5V=1;
    fd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    fd.enable_protocol_interrupt<p_irq>();
    NVIC_SetPriority(fd.irq<p_irq>(), 20);
    NVIC_EnableIRQ(fd.irq<p_irq>());
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
    fd->PSCR=USIC_CH_PSR_ASCMode_TFF_Msk | USIC_CH_PSR_ASCMode_RFF_Msk;
    fd->PCR_ASCMode|=USIC_CH_PCR_ASCMode_FFIEN_Msk;
    putp=0;
    crc=0;
}

void mitsubishi_PQ_t::protocol_handler(void)
{
    auto x=fd->PSR_ASCMode;
    if(x & USIC_CH_PSR_ASCMode_TFF_Msk)
	fd->PSCR=USIC_CH_PSR_ASCMode_TFF_Msk;
    if(x & USIC_CH_PSR_ASCMode_RFF_Msk) {
	int d;
	while((d=fd.rx_fifo())>=0) {
	    rx_buffer[putp++]=d;
	    crc^=d;
	    itm.PORT[7].u16=rx_buffer[putp-1] | (putp<<8);
	}
	if(putp==9 && crc==0) {
	    position=((rx_buffer[2]+(1<<8)*rx_buffer[3] +(1<<12)*rx_buffer[5]
		+(1<<20)*rx_buffer[6]+(1<<28)*rx_buffer[7])<<4)>>4;
	    angle=conv*float(rx_buffer[2]+(1<<8)*rx_buffer[3]);
	    valid=1;
	} else if(putp>=9)
	    valid=0;
	fd->PSCR=USIC_CH_PSR_ASCMode_RFF_Msk;
    }
    NVIC_ClearPendingIRQ(fd.irq<p_irq>());
}

////////////////////////////////////////////////////////////////////////////////
void init_encoder(void)
{
    encoder.set<mitsubishi_PQ_t>();
}

/*******************************************************************************
    Interrupt handlers
*******************************************************************************/

/* Transmit buffer event */
extern "C" void USIC0_0_IRQHandler(void)
{
    static_assert(encoder_t::tb_irq==0, "Transmit should be mapped to IRQ0");
    static_assert(uart::half_duplex(ENC_TXD).UNIT==0, "Invalid unit mapping");
    encoder->tb_handler();
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
