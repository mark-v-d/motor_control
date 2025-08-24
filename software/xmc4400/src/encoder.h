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
    int32_t last_index=0;
    int32_t now;
    int invalid=0;
    int last=0;
    int32_t mask;
    float conv;
protected:
    int trigger_count=0;
public:
    virtual ~encoder_t() {}
    auto get_position_angle() {
	auto c=float(now-time[0])/(time[1]-time[0]);
	int32_t p=std::round((position[1]-position[0])*c+position[0]);
	float a=conv*((p-last_index)&mask);
	now++;
	return std::tuple{p,a};
    }

    auto get_age() const { return now-time[last]; }
    int get_missing() const { return trigger_count; }
    int get_invalid() const { return invalid; }
    int32_t index() const { return last_index; }

    virtual void trigger()=0;
    virtual void rx_handler()=0;
    virtual void tx_handler()=0;
    virtual void protocol_handler()=0;

    static constexpr int p_irq=2;
    static constexpr int rx_irq=1;
    static constexpr int tx_irq=0;
protected:
    void set_angle_conversion(int32_t m, float c) {
	mask=m;
	conv=c;
    }

    void new_position(int32_t p, int32_t i=0) {
	last^=1;
	position[last]=p;
	time[last]=now;
	last_index=i;
    }

    void invalidate() { invalid++; }
};

extern polymorphic_t<encoder_t,116> encoder;

extern posif::qdi32_t<decltype(ENC_A),decltype(ENC_B),decltype(ENC_Z)>
    glass_scale;

void init_encoder();
void set_encoder(int,int);

#endif // ENCODER_H
