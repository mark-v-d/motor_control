#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>
#include <memory>
#include "xmc_uart.h"

class encoder_t {
public:
    virtual ~encoder_t(void) {}
    virtual int32_t position(void)=0;
    virtual float angle(void)=0;
    virtual bool valid(void)=0;

    virtual void trigger(void)=0;
    virtual void half_duplex(void)=0;
    virtual void full_duplex(void)=0;

    // FIXME, we need two seperate (and identical) encoders
    virtual int32_t position2(void) { return 0; } 
    virtual int32_t index2(void) { return 0; }

    static constexpr int hd_irq=1; // half duplex irq/SR output
    static constexpr int fd_irq=0; // full duplex irq/SR output

protected:
    static void init_full_duplex(const XMC_UART_CH_CONFIG_t &uart_config);
    static void init_half_duplex(const XMC_UART_CH_CONFIG_t &uart_config);
};

extern std::unique_ptr<encoder_t> encoder;

void init_encoder(void);

#endif // ENCODER_H
