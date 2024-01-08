#include "rtapi.h"
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "hal.h"

#include "ss.h"

static int comp_id;

#ifdef MODULE_INFO
MODULE_INFO(linuxcnc, const_cast<char*>("component:statespace:"));
//MODULE_INFO(linuxcnc, const_cast<char*>("pin:out:float:0:out::None:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("param:value:float:0:r::1.0:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("funct:sync:1:"));
MODULE_INFO(linuxcnc, const_cast<char*>("license:GPL"));
MODULE_LICENSE("GPL");
#endif // MODULE_INFO

#include <string>
using namespace std::string_literals;

constexpr int order=3;
constexpr int inputs=2;
constexpr int outputs=1;

/******************************************************************************/
class speed_voltage_t {
    // Inputs
    hal_float_t *speed;
    hal_float_t *V_per_speed;
    hal_float_t *Vservo;	// Supply voltage of motor drive
    hal_float_t *Vrotor;	// Imaginary part off rotor voltage
    hal_float_t *Imax;		// Maximum motor current
    hal_float_t *Irotor;	// Actual motor current
    hal_float_t *Vdelta;	// Volts per second
    hal_float_t *limit_max;	// The maximum duty cycle for the imaginary part
    hal_s32_t	*position;	// Encoder of spindle
    hal_u32_t	*position_per_rev;// Encoder increments per revolution
    hal_float_t *filter;	// filter gain
    hal_bit_t	*index_enable;	// For threading
    hal_bit_t	*index;		// For threading

    // Outputs
    hal_float_t *Iout;		// Motor current (imaginary part)
    hal_float_t *limit;		// duty cycle limit (imaginary part)
    hal_bit_t	*at_speed;	// Setpoint achieved
    hal_float_t	*revs;		// Number of revolutions
    hal_float_t	*speed_fb;	// Measured speed (rps)
    hal_float_t	*I_fb;		// Measured current
    hal_float_t	*speed_rpm;	// Measured speed (rpm)

    double Vcurrent;		// Current motor voltage
    double Vmeasured;		// Measured motor voltage (filtered)
    int32_t old_position;	// Used for speed measurement
    double Speed_measured=0;	// Speed measured by encoder (filtered)
    double current_measured=0;	// Absolute measured current (for load meter)
    double revs_offset;	// by index

public:
    bool init(int i) {
	Vcurrent=0;
	Vmeasured=0;
	old_position=0;
	Speed_measured=0;
	current_measured=0;

	std::string prefix="speed_controller."+std::to_string(i)+".";
	auto pin=[&](hal_pin_dir_t dir,std::string name, auto p) {
	    *p=nullptr;
	    using T=decltype(p);
	    if constexpr(std::is_same_v<T,hal_float_t**>)
		return hal_pin_float_new((prefix+name).c_str(),dir,p,comp_id);
	    else if constexpr(std::is_same_v<T,hal_s32_t**>)
		return hal_pin_s32_new((prefix+name).c_str(),dir,p,comp_id);
	    else if constexpr(std::is_same_v<T,hal_u32_t**>)
		return hal_pin_u32_new((prefix+name).c_str(),dir,p,comp_id);
	    else if constexpr(std::is_same_v<T,hal_bit_t**>)
		return hal_pin_bit_new((prefix+name).c_str(),dir,p,comp_id);
	    else
		return 1;
	};
	return
	    pin(HAL_IN, "speed", &speed) ||
	    pin(HAL_IN, "V_per_speed", &V_per_speed) ||
	    pin(HAL_IN, "Vservo", &Vservo) ||
	    pin(HAL_IN, "Vrotor", &Vrotor) ||
	    pin(HAL_IN, "Imax", &Imax) ||
	    pin(HAL_IN, "Irotor", &Irotor) ||
	    pin(HAL_IN, "Vdelta", &Vdelta) ||
	    pin(HAL_IN, "limit_max", &limit_max) ||
	    pin(HAL_IN, "position", &position) ||
	    pin(HAL_IN, "position_per_rev", &position_per_rev) ||
	    pin(HAL_IN, "filter", &filter) ||
	    pin(HAL_IO, "index_enable", &index_enable) ||
	    pin(HAL_IN, "index", &index) ||
	    pin(HAL_OUT, "Iout", &Iout) ||
	    pin(HAL_OUT, "limit", &limit) ||
	    pin(HAL_OUT, "at_speed", &at_speed) ||
	    pin(HAL_OUT, "revs", &revs) ||
	    pin(HAL_OUT, "speed_fb", &speed_fb) ||
	    pin(HAL_OUT, "speed_rpm", &speed_rpm) ||
	    pin(HAL_OUT, "I_fb", &I_fb)
	    ;
    }

    auto compute(long period) {
	double Vset=(*speed)*(*V_per_speed);
	double dV=(*Vdelta)*period/1e9;

	double ppr=double(*position_per_rev);
	int32_t pos=*position;
	*revs=pos/ppr-revs_offset;

	double dr=pos-old_position;
	old_position=pos;
	double gain=*filter;
	Speed_measured+=gain*(dr/ppr/period*1e9-Speed_measured);
	*speed_fb=Speed_measured;
	*speed_rpm=std::abs(60*Speed_measured);

	current_measured+=gain*(std::abs(*Irotor)-current_measured);
	*I_fb=current_measured;

	Vmeasured+=gain*((*Vrotor)*(*Vservo)-Vmeasured);
	double Iset=Vcurrent<0? -(*Imax):(*Imax);
	double duty=std::abs(Vcurrent/(*Vservo));
	if(Vset>Vcurrent) {
	    if(Vcurrent<-1) {
		// Decelerate
		Vcurrent=Vmeasured;
		Iset=0;
		duty=*limit_max;
	    } else {
		// Accelerate
		Vcurrent+=dV;
		Iset=*Imax;
		duty=std::abs(Vcurrent/(*Vservo));
	    }
	    if(Vcurrent>Vset)
		Vcurrent=Vset;
	} else if(Vset<Vcurrent) {
	    if(Vcurrent>1) {
		// Decelerate
		Vcurrent=Vmeasured;
		Iset=0;
		duty=*limit_max;
	    } else {
		// Accelerate
		Vcurrent-=dV;
		Iset=-*Imax;
		duty=std::abs(Vcurrent/(*Vservo));
	    }
	}

	duty=std::min(double(*limit_max),duty);
	duty=std::max(0.0,duty);

	if(*index_enable && *index) {
	    revs_offset=(0xffffc000&pos)/ppr;
	    *index_enable=0;
	}


	if(*Imax==0)
	    duty=*limit_max;

	*Iout=Iset;
	*limit=duty;
	*at_speed=std::abs(Vmeasured-Vset)<1.0;
    }
};


std::vector<speed_voltage_t*> state;

static void compute(void *p, long period)
{
    for(auto x: state)
	x->compute(period);
}


////////////////////////////////////////////////////////////////////////////////
static int count=0;
RTAPI_MP_INT(count, "number of controllers");

extern "C" int rtapi_app_main(void)
{
    int r = 0;
    int i;
    comp_id = hal_init("speed_controller");
    if(comp_id < 0)
	return comp_id;

    for(int i=0; !r && i<count; i++) {
        auto p=reinterpret_cast<speed_voltage_t*>(
	    hal_malloc(sizeof(speed_voltage_t)));
        r=p->init(i);
	state.push_back(p);
    }

    if(!r)
	r = hal_export_funct("speed_controller.compute", compute, NULL,
	    1, 0, comp_id);
    if(r) {
        hal_exit(comp_id);
    } else {
        hal_ready(comp_id);
    }
    return r;
}

extern "C" void rtapi_app_exit(void)
{
    hal_exit(comp_id);
}
