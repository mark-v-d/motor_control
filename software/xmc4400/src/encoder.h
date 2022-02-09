#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>
#include "polymorphic.h"
#include "xmc_uart.h"

class encoder_t {
public:
    virtual ~encoder_t(void) {}
    virtual int32_t position(void)=0;
    virtual float angle(void)=0;
    virtual bool valid(void)=0;

    virtual void trigger(void)=0;
    virtual void rx_handler(void)=0;
    virtual void tb_handler(void)=0;
    virtual void protocol_handler(void)=0;

    static constexpr int p_irq=2;
    static constexpr int rx_irq=1;
    static constexpr int tb_irq=0;
};

extern polymorphic_t<encoder_t,1024> encoder;

void init_encoder(void);

#endif // ENCODER_H
