#include "encoder.h"
#include "hardware.h"

#include "xmc_scu.h"

#include <atomic>
#include <math.h>

std::unique_ptr<encoder_t> encoder(new dummy_encoder_t);

extern std::atomic<uint32_t> counter;

constexpr auto PI=acos(-1);

/*******************************************************************************
    Dummy encoder, encoder is initially of this type and it does nothing
    and is never valid.
*******************************************************************************/
uint32_t dummy_encoder_t::position(void) { return 0;} 
float dummy_encoder_t::angle(void) { return 0.0; }
bool dummy_encoder_t::valid(void) { return false;}

void dummy_encoder_t::trigger(void) {}
void dummy_encoder_t::half_duplex(void) {}
void dummy_encoder_t::full_duplex(void) {}

/*******************************************************************************
    Mitsubishi base class
*******************************************************************************/
class mitsubishi_encoder_t:public encoder_t {
protected:
    int putp;
    uint8_t rx_buffer[16];
public:
    virtual uint32_t position(void) { return 0; }
    virtual float angle(void) { return 0.0F; }
    virtual bool valid(void) { return false; }

    virtual void trigger(void) {}
    virtual void half_duplex(void);
    virtual void full_duplex(void);

    static int detect(void);
private:
    void serial_tx(uint8_t);
};

void mitsubishi_encoder_t::half_duplex(void)
{
    ENC_DIR=0;
    XMC_USIC_CH_t *usic=ENC_TXD;
    uint32_t reason=usic->PSR;
    if(reason & USIC_CH_PSR_ASCMode_RFF_Msk)
	rx_buffer[putp++]=usic->RBUF;
    usic->PSCR=reason;
}

void mitsubishi_encoder_t::full_duplex(void)
{
    // static_assert(uart_number(ENC_RXD2)==1, "FIXME");
    u1c1.PSCR=u1c1.PSR;
    rx_buffer[putp++]=u1c1.RBUF;
}

void mitsubishi_encoder_t::serial_tx(uint8_t data)
{
    ENC_DIR=1;
    putp=0;
    ENC_TXD=data;
    uint32_t old_counter=counter;
    while(counter<old_counter+1)
	;
}

/* HC-MFS13-S13 motor *********************************************************/
class mitsubishi_MFS13_t:public mitsubishi_encoder_t 
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<20);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
public:
    virtual uint32_t position(void);
    virtual float angle(void); 
    virtual bool valid(void);

    virtual void trigger(void);
};

float mitsubishi_MFS13_t::angle(void)
{
    uint32_t encoder=rx_buffer[2]+(1<<8)*rx_buffer[3]+(1<<16)*rx_buffer[4];
    return conv*float(encoder);
}

uint32_t mitsubishi_MFS13_t::position(void)
{
    return rx_buffer[2]+(1<<8)*rx_buffer[3]+(1<<16)*rx_buffer[4]
	+(1<<20)*rx_buffer[5]+(1<<28)*(rx_buffer[6]*0x0f);
}

bool mitsubishi_MFS13_t::valid(void)
{
    if(putp!=9)
	return false;
    uint8_t crc=0;
    for(int i=0;i<putp;i++)
	crc^=rx_buffer[i];
    return crc==0;
}

void mitsubishi_MFS13_t::trigger(void)
{
    ENC_DIR=1;
    putp=0;
    ENC_TXD=0x1a;
} 

/* HC-PQ[24]3 motor ***********************************************************/
class mitsubishi_PQ_t:public mitsubishi_encoder_t 
{
    constexpr static int poles=4;
    constexpr static int increments_per_revolution=(1<<12);
    constexpr static float conv=2.0*PI*poles/increments_per_revolution;
public:
    virtual uint32_t position(void);
    virtual float angle(void); 
    virtual bool valid(void);

    virtual void trigger(void);
};

float mitsubishi_PQ_t::angle(void)
{
    uint32_t encoder=rx_buffer[2]+(1<<8)*rx_buffer[3];
    return conv*float(encoder);
}

uint32_t mitsubishi_PQ_t::position(void)
{
    return rx_buffer[2]+(1<<8)*rx_buffer[3]
	+(1<<12)*rx_buffer[5]+(1<<20)*rx_buffer[6]+(1<<28)*rx_buffer[7];
}

bool mitsubishi_PQ_t::valid(void)
{
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
    putp=0;
    ENC_TXD=0x1a;
} 

/******************************************************************************/
int mitsubishi_encoder_t::detect(void)
{
    mitsubishi_encoder_t *p=new mitsubishi_encoder_t;
    encoder=std::unique_ptr<mitsubishi_encoder_t>(p);

    XMC_UART_CH_CONFIG_t uart_config = {
	.baudrate=2500000,
	.data_bits=8U,
	.frame_length=0,
	.stop_bits=1,
	.oversampling=0,
	.parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
    };

    XMC_UART_CH_Init(ENC_TXD, &uart_config);
    XMC_UART_CH_SetInputSource(ENC_TXD, 
	XMC_UART_CH_INPUT_RXD, USIC0_C0_DX0_P1_4);
    XMC_UART_CH_EnableEvent(ENC_TXD, 
	XMC_UART_CH_EVENT_FRAME_FINISHED );
    XMC_UART_CH_SelectInterruptNodePointer(ENC_TXD, 
	XMC_UART_CH_INTERRUPT_NODE_POINTER_PROTOCOL, 1);
    XMC_UART_CH_Start(ENC_TXD);
    // Protocol interrupt
    NVIC_SetPriority(USIC0_1_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC0_1_IRQn);
    NVIC_EnableIRQ(USIC0_1_IRQn);


    // Powerup and wait
    p->serial_tx(0x1a);
    ENC_5V=1;
    uint32_t old_counter=counter;
    while(counter<old_counter+450)
	;

    // First try to use half-duplex mode
    p->serial_tx(0x1a);
    if(p->putp==9) {
	encoder=std::unique_ptr<mitsubishi_MFS13_t>(new mitsubishi_MFS13_t);
	return 0;
    }

    // We don't need to disable the transmitter in full-duplex
    XMC_UART_CH_DisableEvent(ENC_TXD, XMC_UART_CH_EVENT_FRAME_FINISHED );
    NVIC_DisableIRQ(USIC0_1_IRQn);

    XMC_UART_CH_Init(XMC_UART1_CH1, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART1_CH1, 
	XMC_UART_CH_INPUT_RXD, USIC1_C1_DX0_P0_0);
    XMC_UART_CH_EnableInputInversion(XMC_UART1_CH1,XMC_UART_CH_INPUT_RXD);
    XMC_UART_CH_EnableEvent(XMC_UART1_CH1, 
	XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    XMC_UART_CH_SelectInterruptNodePointer(XMC_UART1_CH1, 
	XMC_UART_CH_INTERRUPT_NODE_POINTER_RECEIVE, 0);
    XMC_UART_CH_Start(XMC_UART1_CH1);
    // Receive on channel 1
    NVIC_SetPriority(USIC1_0_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC1_0_IRQn);
    NVIC_EnableIRQ(USIC1_0_IRQn);

    p->serial_tx(0x1a);
    if(p->putp!=9) {
	ENC_5V=0;
	ENC_DIR=0;
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC0);
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	NVIC_DisableIRQ(USIC1_0_IRQn);
	return 1;
    }

    encoder=std::unique_ptr<mitsubishi_PQ_t>(new mitsubishi_PQ_t);
    return 0;
}


void init_encoder(void)
{
    if(mitsubishi_encoder_t::detect())
	return;
}
