#ifndef ENCODER_H
#define ENCODER_H
#include <cstdint>
#include <tuple>
#include "polymorphic.h"
#include "xmc_uart.h"

class encoder_t {
protected:
    uint32_t position;
    float angle;
    int valid=0;
public:
    virtual ~encoder_t(void) {}
    auto get_pav(void) { return std::tuple{position,angle,valid}; }

    virtual void trigger(void)=0;
    virtual void rx_handler(void)=0;
    virtual void tb_handler(void)=0;
    virtual void protocol_handler(void)=0;

    static constexpr int p_irq=2;
    static constexpr int rx_irq=1;
    static constexpr int tb_irq=0;
};

#include "posif.h"
#include "hardware.h"


extern polymorphic_t<encoder_t,1024> encoder;

extern posif::qdi32_t<decltype(ENC_A),decltype(ENC_B),decltype(ENC_Z)> glass_scale;

void init_encoder(void);

#endif // ENCODER_H
