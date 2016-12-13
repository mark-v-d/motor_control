#include "encoder.h"
#include "hardware.h"

#include "xmc_scu.h"

#include <atomic>

void init_pos_dummy(position_t*,uint8_t*) {}
void (*init_pos)(position_t*,uint8_t*)=init_pos_dummy;


void position_MFS13_13(position_t *pos, uint8_t *p)
{
    const float conv=2.0*3.14159265358979/262144.0;
    pos->start=p[0];
    pos->state=p[1];
    pos->encoder=p[2]+256*p[3]+65536*p[4];
    pos->angle=conv*pos->encoder;
    pos->rotation=p[5]+0x100*p[6]+0x10000*p[7];
    pos->crc=0;
    for(int i=0;i<9;i++)
	pos->crc^=p[i];
}

void position_HC_PQ23(position_t *pos, uint8_t *p) 
{
    const float PI=3.14159265358979;
    pos->start=p[0];
    pos->state=p[1];
    pos->encoder=(p[2]+256*p[3]);
    pos->angle=float(2*PI/1024.0)*pos->encoder;
    pos->rotation=p[5]+0x100*p[6];
    pos->crc=0;
    for(int i=0;i<9;i++)
	pos->crc^=p[i];
}

extern uint8_t rx_buffer[20] __attribute__((section ("ETH_RAM")));
extern volatile int putp;
extern std::atomic<int> counter;

void serial_tx(uint8_t data)
{
    ENC_DIR=1;
    putp=0;
    u0c0.TBUF[0]=0x1a;
    uint32_t old_counter=counter;
    while(counter<old_counter+1)
	;
}

/*
    Configure the Mitsubishi encoder interface
    returns 1 if failed.
*/
int init_mitsubishi(void)
{
    XMC_UART_CH_CONFIG_t uart_config = {
	.baudrate=2500000,
	.data_bits=8U,
	.frame_length=0,
	.stop_bits=1,
	.oversampling=0,
	.parity_mode=XMC_USIC_CH_PARITY_MODE_NONE
    };
    XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);
    XMC_UART_CH_SetInputSource(XMC_UART0_CH0, 
	XMC_UART_CH_INPUT_RXD, USIC0_C0_DX0_P1_4);
    XMC_UART_CH_EnableEvent(XMC_UART0_CH0, 
	XMC_UART_CH_EVENT_FRAME_FINISHED );
    XMC_UART_CH_SelectInterruptNodePointer(XMC_UART0_CH0, 
	XMC_UART_CH_INTERRUPT_NODE_POINTER_PROTOCOL, 1);
    XMC_UART_CH_Start(XMC_UART0_CH0);
    // Protocol interrupt
    NVIC_SetPriority(USIC0_1_IRQn,  0);
    NVIC_ClearPendingIRQ(USIC0_1_IRQn);
    NVIC_EnableIRQ(USIC0_1_IRQn);

    // Powerup and wait
    serial_tx(0x1a);
    ENC_5V=1;
    uint32_t old_counter=counter;
    while(counter<old_counter+450)
	;

    // First try to use half-duplex mode
    serial_tx(0x1a);
    if(putp==9) {
	init_pos=position_MFS13_13;
	return 0;
    }

    // We don't need to disable the transmitter in full-duplex
    XMC_UART_CH_DisableEvent(XMC_UART0_CH0, 
	XMC_UART_CH_EVENT_FRAME_FINISHED );
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

    serial_tx(0x1a);
    if(putp!=9) {
	ENC_5V=0;
	ENC_DIR=0;
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC0);
	XMC_SCU_RESET_AssertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_USIC1);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
	XMC_SCU_CLOCK_GatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC1);
	NVIC_DisableIRQ(USIC1_0_IRQn);
	return 1; 
    }

    init_pos=position_HC_PQ23;
    return 0;
}
