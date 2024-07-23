#ifndef ENCODER_H
#define ENCODER_H
#include <cstdint>
#include <tuple>

#include "hardware.h"
#include "posif.h"
#include "polymorphic.h"

class encoder_t {
    int32_t position[2];
    int32_t time[2];
    int32_t now;
    int valid=0;
    int last=0;
    int32_t mask;
    float conv;
public:
    virtual ~encoder_t(void) {}
    auto get_pav(void) {
	auto c=1.0f/(time[1]-time[0])*(now-time[0]);
	int32_t p=std::round((position[1]-position[0])*c+position[0]);
	float a=conv*(p&mask);
	now++;
	return std::tuple{p,a,valid};
    }

    virtual void trigger(void)=0;
    virtual void rx_handler(void)=0;
    virtual void tx_handler(void)=0;
    virtual void protocol_handler(void)=0;

    static constexpr int p_irq=2;
    static constexpr int rx_irq=1;
    static constexpr int tx_irq=0;
protected:
    void new_position(int32_t p, float c, int32_t m) {
	last^=1;
	position[last]=p;
	time[last]=now;
	conv=c;
	mask=m;

	valid=1;
    }

    void invalidate() { valid=0; }
};

extern polymorphic_t<encoder_t,80> encoder;

extern posif::qdi32_t<decltype(ENC_A),decltype(ENC_B),decltype(ENC_Z)>
    glass_scale;

struct encoder_parameters_t {
    int poles;
};

void init_encoder(void);
void set_encoder(int,int);

extern uint8_t rx_buffer[16];
extern uint8_t tx_buffer[8];
extern uint8_t command;
extern uint8_t addr;

#endif // ENCODER_H
