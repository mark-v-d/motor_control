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
    hal_float_t *speed;		// Setpoint for speed
    hal_float_t *acceleration;	// Limit acceleration

    hal_float_t *Vservo;	// Supply voltage of motor drive
    hal_float_t *Vrotor;	// Imaginary part off rotor voltage
    hal_float_t *Imax;		// Maximum motor current
    hal_float_t *Irotor[2];	// Actual motor current

    hal_s32_t	*position;	// Encoder of spindle
    hal_u32_t	*position_per_rev;// Encoder increments per revolution
    hal_bit_t	*index_enable;	// For threading
    hal_bit_t	*index;		// For threading

    hal_float_t *gain_P;	// proportional gain
    hal_float_t *gain_I;	// integrating gain
    hal_float_t *gain_L;	// limiting gain

    hal_float_t *emf_start;	// Minimal speed for EMF reduction
    hal_float_t *emf_scale;	// EMF reduction scaling


    // Outputs
    hal_float_t *Iout[2];	// Motor current
    hal_bit_t	*at_speed;	// Setpoint achieved
    hal_float_t	*revs;		// Number of revolutions
    hal_float_t	*speed_fb;	// Measured speed (rps)
    hal_float_t	*I_fb;		// Measured current (for display)
    hal_float_t	*V_fb;		// Measured Voltage
    hal_float_t	*speed_rpm;	// Measured speed (rpm)

    hal_float_t *debug;

    int32_t old_position;	// Used for speed measurement
    double revs_offset;	// by index
    double integrator;
    double current_speed;

    std::array<int32_t,90> pos_buf;
    std::array<double,90> I_buf;
    std::array<double,90> V_buf;
    int pos_idx;
    double measured_speed;

public:
    bool init(int i) {
	old_position=0;

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
	    pin(HAL_IN, "acceleration", &acceleration) ||

	    pin(HAL_IN, "Vservo", &Vservo) ||
	    pin(HAL_IN, "Vrotor", &Vrotor) ||
	    pin(HAL_IN, "Imax", &Imax) ||
	    pin(HAL_IN, "Irotor-0", &Irotor[0]) ||
	    pin(HAL_IN, "Irotor-1", &Irotor[1]) ||

	    pin(HAL_IN, "gain_P", &gain_P) ||
	    pin(HAL_IN, "gain_I", &gain_I) ||
	    pin(HAL_IN, "gain_L", &gain_L) ||

	    pin(HAL_IN, "emf_start", &emf_start) ||
	    pin(HAL_IN, "emf_scale", &emf_scale) ||

	    pin(HAL_IN, "position", &position) ||
	    pin(HAL_IN, "position_per_rev", &position_per_rev) ||
	    pin(HAL_IO, "index_enable", &index_enable) ||
	    pin(HAL_IN, "index", &index) ||

	    pin(HAL_OUT, "Iout-0", &Iout[0]) ||
	    pin(HAL_OUT, "Iout-1", &Iout[1]) ||
	    pin(HAL_OUT, "at_speed", &at_speed) ||
	    pin(HAL_OUT, "revs", &revs) ||
	    pin(HAL_OUT, "speed_fb", &speed_fb) ||
	    pin(HAL_OUT, "speed_rpm", &speed_rpm) ||
	    pin(HAL_OUT, "I_fb", &I_fb) ||
	    pin(HAL_OUT, "V_fb", &V_fb) ||
	    pin(HAL_OUT, "debug", &debug) ||
	    0;
    }

    auto compute(long period) {
	double ppr=double(*position_per_rev);
	int32_t pos=*position;
	*revs=pos/ppr-revs_offset;


	// acceleration limit
	double speed_in=*speed;
	if(current_speed<speed_in) {
	    current_speed+=*acceleration;
	    current_speed=std::min(speed_in,current_speed);
	} else if(current_speed>speed_in) {
	    current_speed-=*acceleration;
	    current_speed=std::max(speed_in,current_speed);
	}

	double scale=(1e9/period)/pos_buf.size()/(*position_per_rev);
	measured_speed=double(pos-pos_buf[pos_idx])*scale;
	*I_fb-=I_buf[pos_idx]/I_buf.size();
	I_buf[pos_idx]=std::abs(std::complex<double>{*Irotor[0],*Irotor[1]});
	*I_fb+=I_buf[pos_idx]/I_buf.size();
	*V_fb-=V_buf[pos_idx]/V_buf.size();
	V_buf[pos_idx]=*Vrotor;
	*V_fb+=V_buf[pos_idx]/V_buf.size();
	pos_buf[pos_idx]=pos;
	if(++pos_idx>=pos_buf.size())
	    pos_idx=0;

	if(std::abs(current_speed)>*emf_start)
	    *Iout[0]=*emf_scale*(std::abs(current_speed)-*emf_start);
	else
	    *Iout[0]=0;

	// Determine current limits
	double limit_max=std::sqrt(*Imax**Imax-*Iout[0]**Iout[0]);
	double limit_min=-limit_max;
	double Vmeas=(*Vrotor)*(*Vservo);
	if(Vmeas>5.0)
	    limit_min=0;
	else if(Vmeas<-5.0)
	    limit_max=0;

	double kP=*gain_P;
	double kI=*gain_I;
	double kL=*gain_L;

	double error=current_speed-measured_speed;
	double unlimited=error*kP+integrator;
	integrator+=kI*error;
	*debug=old_position-pos;
	old_position=pos;

	double limited=std::min(limit_max,std::max(limit_min,unlimited));
	if(speed_in==0)
	    limited=0;
	double correction=limited-unlimited;
	integrator+=correction*kL;

	*Iout[1]=limited;
	*at_speed=std::abs(error)<0.1 && std::abs(speed_in-measured_speed)<0.1;

	if(*index_enable && *index) {
	    revs_offset=(0xffffc000&pos)/ppr;
	    *index_enable=0;
	}

	*speed_fb=measured_speed;
	*speed_rpm=std::abs(60*measured_speed);
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
