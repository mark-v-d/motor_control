#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>
#include <memory>

class encoder_t {
public:
    virtual ~encoder_t(void) {}
    virtual uint32_t position(void)=0;
    virtual float angle(void)=0;
    virtual bool valid(void)=0;

    virtual void trigger(void)=0;
    virtual void half_duplex(void)=0;
    virtual void full_duplex(void)=0;
};

extern std::unique_ptr<encoder_t> encoder;

void init_encoder(void);

#endif // ENCODER_H
