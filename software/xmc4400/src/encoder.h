#ifndef ENCODER_H
#define ENCODER_H
#include <cstdint>
#include <tuple>

#include "hardware.h"
#include "posif.h"
#include "polymorphic.h"

class encoder_t {
protected:
    int32_t position;
    float angle;
    int valid=0;
public:
    virtual ~encoder_t(void) {}
    auto get_pav(void) { return std::tuple{position,angle,valid}; }

    virtual void trigger(void)=0;
    virtual void rx_handler(void)=0;
    virtual void tx_handler(void)=0;
    virtual void protocol_handler(void)=0;

    static constexpr int p_irq=2;
    static constexpr int rx_irq=1;
    static constexpr int tx_irq=0;
};

extern polymorphic_t<encoder_t,64> encoder;

extern posif::qdi32_t<decltype(ENC_A),decltype(ENC_B),decltype(ENC_Z)>
    glass_scale;

void init_encoder(void);

extern uint8_t rx_buffer[16];
extern uint8_t tx_buffer[8];
extern uint8_t command;
extern uint8_t addr;

#endif // ENCODER_H
