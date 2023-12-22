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
struct comp_state {
    hal_s32_t *in[inputs];
    hal_float_t *scale[inputs];
    hal_float_t *setpoint;
    hal_bit_t *enable;
    hal_float_t *error[inputs];
    hal_float_t *out[outputs];
    hal_float_t *position[inputs];

    std::array<hal_float_t*,order*order> A;
    std::array<hal_float_t*,order*inputs> B;
    std::array<hal_float_t*,order*outputs> C;
    std::array<hal_float_t*,inputs*outputs> D;
    std::array<hal_float_t*,order*outputs> K;
    std::array<hal_float_t*,outputs> limit;

    double old_setpoint;
    ss_t<order,inputs,outputs> controller;

    int init(int i) {
	char buffer[100];
	sprintf(buffer,"statespace_3.%d.",i);
	std::string prefix{buffer};

	auto pin=[&](hal_pin_dir_t dir,std::string name, auto p) {
	    *p=nullptr;
	    using T=decltype(p);
	    if constexpr(std::is_same_v<T,hal_float_t**>)
		return hal_pin_float_new((prefix+name).c_str(),dir,p,comp_id);
	    else if constexpr(std::is_same_v<T,hal_s32_t**>)
		return hal_pin_s32_new((prefix+name).c_str(),dir,p,comp_id);
	    else if constexpr(std::is_same_v<T,hal_bit_t**>)
		return hal_pin_bit_new((prefix+name).c_str(),dir,p,comp_id);
	    else
		return 1;
	};
	int fail=
	    pin(HAL_IN, "enable", &enable) ||
	    pin(HAL_OUT, "out", &out[0]) ||
	    pin(HAL_IN,"setpoint",&setpoint);

	// Initialise the A,B,C,K matrices
	for(int i=0; i<order; i++) {
	    for(int j=0; !fail && j<order; j++) {
		char buffer[100];
		sprintf(buffer,"A-%d-%d",i,j);
		fail=pin(HAL_IN,buffer,&A[order*i+j]);
	    }
	    for(int j=0; !fail && j<inputs; j++) {
		char buffer[100];
		sprintf(buffer,"B-%d-%d",i,j);
		fail=pin(HAL_IN,buffer,&B[inputs*i+j]);
	    }
	    for(int j=0; !fail && j<outputs; j++) {
		char buffer[100];
		sprintf(buffer,"K-%d-%d",i,j);
		fail=pin(HAL_IN,buffer,&K[outputs*i+j]);
	    }
	}
	for(int i=0; i<outputs; i++) {
	    for(int j=0; !fail && j<order; j++) {
		char buffer[100];
		sprintf(buffer,"C-%d-%d",i,j);
		fail=pin(HAL_IN,buffer,&C[order*i+j]);
	    }
	    for(int j=0; !fail && j<inputs; j++) {
		char buffer[100];
		sprintf(buffer,"D-%d-%d",i,j);
		fail=pin(HAL_IN,buffer,&D[inputs*i+j]);
	    }
	    if(!fail) {
		char buffer[100];
		sprintf(buffer,"limit-%d",i);
		fail=pin(HAL_IN,buffer,&limit[i]);
	    }
	}
	for(int i=0; !fail && i<inputs; i++) {
		char buffer[100];
		sprintf(buffer,"error-%d",i);
		std::string n=std::to_string(i);
		fail=pin(HAL_OUT,"error-"s+n, &error[i]) ||
		    pin(HAL_IN, "in-"s+n, &in[i]) ||
		    pin(HAL_IN, "scale-"s+n, &scale[i]) ||
		    pin(HAL_OUT, "position-"s+n, &position[i]);
	}
	return fail;
    }

    void compute() {
	for(int i=0; i<order; i++) {
	    for(int j=0; j<order; j++)
		controller.A(i,j)=*A[order*i+j];
	    for(int j=0; j<inputs; j++)
		controller.B(i,j)=*B[inputs*i+j];
	    for(int j=0; j<outputs; j++)
		controller.K(i,j)=*K[outputs*i+j];
	}
	for(int i=0; i<outputs; i++) {
	    for(int j=0; j<order; j++)
		controller.C(i,j)=*C[order*i+j];
	    for(int j=0; j<inputs; j++)
		controller.D(i,j)=*D[inputs*i+j];
	    if(*enable)
		controller.limit(i)=*limit[i];
	    else
		controller.limit(i)=0;
	}
	decltype(controller)::input_t inp;
	for(int i=0; i<inputs; i++) {
	    double SCALE=*scale[i];
	    double IN=*in[i];
	    *position[i]=IN/SCALE;
	    double ERROR=(*setpoint*SCALE-IN);
	    *error[i]=ERROR/SCALE;
	    if(old_setpoint==*setpoint)
		inp(i)=round(ERROR);
	    else
		inp(i)=ERROR;
	}
	old_setpoint=*setpoint;
	auto r=controller.compute(inp);
	*out[0]=r(0);
    }
};

std::vector<comp_state*> state;

static void compute(void *p, long period)
{
    for(auto x: state)
	x->compute();
}


////////////////////////////////////////////////////////////////////////////////
static int count=0;
RTAPI_MP_INT(count, "number of controllers");

extern "C" int rtapi_app_main(void)
{
    int r = 0;
    int i;
    comp_id = hal_init("statespace_3");
    if(comp_id < 0)
	return comp_id;

    for(int i=0; !r && i<count; i++) {
        auto p=reinterpret_cast<comp_state*>(hal_malloc(sizeof(comp_state)));
        r=p->init(i);
	state.push_back(p);
    }

    // Export the send-sync function
    if(!r)
	r = hal_export_funct("statespace_3.compute", compute,NULL, 1, 0, comp_id);
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
