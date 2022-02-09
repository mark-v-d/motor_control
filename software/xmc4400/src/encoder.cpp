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

    virtual int32_t position(void) { return 0;}
    virtual float angle(void) { return 0.0;}
    virtual bool valid(void) { return false;}

    virtual void trigger(void) {}
    virtual void rx_handler(void) {}
    virtual void tb_handler(void) {}
    virtual void protocol_handler(void) {}
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
public:
    mitsubishi_MFS13_t(void);
    virtual ~mitsubishi_MFS13_t(void);
    virtual int32_t position(void);
    virtual float angle(void);
    virtual bool valid(void);

    virtual void trigger(void);
    virtual void rx_handler(void) {}
    virtual void tb_handler(void) {}
    virtual void protocol_handler(void);
};

mitsubishi_MFS13_t::mitsubishi_MFS13_t(void)
{
    ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    ENC_DIR=0;
    ENC_5V=1;
    hd.init(baudrate);
    uart::fifo_configure<0,16>(hd);

    hd.enable_protocol_interrupt<p_irq>();
    NVIC_SetPriority(hd.irq<p_irq>(), 0);
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

void mitsubishi_MFS13_t::protocol_handler(void)
{
    itm.PORT[0].u32=hd->PSR_ASCMode;
    if(hd->PSR_ASCMode & USIC_CH_PSR_ASCMode_TFF_Msk) {
	hd->PSCR=USIC_CH_PSR_ASCMode_TFF_Msk | USIC_CH_PSR_ASCMode_RFF_Msk;
	hd->PCR_ASCMode&=~USIC_CH_PCR_ASCMode_FFIEN_Msk;
	ENC_DIR=0;
	ENC_TXD.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    }
    NVIC_ClearPendingIRQ(fd.irq<p_irq>());
}

void init_encoder(void)
{
    encoder.set<mitsubishi_MFS13_t>();
}
