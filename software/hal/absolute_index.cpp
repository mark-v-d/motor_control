#include "rtapi.h"
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "hal.h"
#include <vector>
#include <cmath>

static int comp_id;

/* Do absolute homing using the POSITION_FILE and the index of the encoder
*/

#ifdef MODULE_INFO
MODULE_DESCRIPTION("Pseudo absolute encoder using POSITION_FILE and the encoder index")
MODULE_LICENSE("GPL");
#endif // MODULE_INFO

#include <string>
using namespace std::string_literals;

/******************************************************************************/
class absolute_index_t {
    // Inputs
    hal_float_t *pos_fb;	// Position from joint.?.pos-fb
    hal_float_t *scale;		// Factor to scale mm to encoder units

    hal_s32_t	*position_in;	// Encoder counter
    hal_s32_t	*index;		// 0 -> index not seen, 1 -> index seen
    hal_s32_t	*counts_per_index; // counts per index (only 0x10000 is tested)

    hal_bit_t	*enable;	// joint.?.amp-enable-out
    hal_bit_t	*is_homed;	// joint.?.is-homed

    // Outputs
    hal_u32_t *debug_fb;
    hal_u32_t *debug_out;
    hal_u32_t *debug_real;
    hal_float_t	*offset;	// Measured distance between POSITION_FILE
				// and the actual position from the encoder
    hal_s32_t	*position_out;	// encoder counter corrected to absolute
				// position
    hal_s32_t	*index_count;	// Used to trigger halscope
    hal_bit_t	*home;		// joint.?.home Used to home the joint
				// when the index has been passed

    int32_t correction;
    int32_t position_d1;
    int32_t position_d2;
    int32_t timer;
    int32_t count;
    uint32_t was_homed;
public:
    bool init(int i) {
	std::string prefix="absolute_index."+std::to_string(i)+".";
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
	timer=10000;
	was_homed=0;
	return
	    pin(HAL_IN, "pos-fb", &pos_fb) ||
	    pin(HAL_IN, "scale", &scale) ||

	    pin(HAL_IN, "position-in", &position_in) ||
	    pin(HAL_IN, "index", &index) ||
	    pin(HAL_IN, "counts_per_index", &counts_per_index) ||

	    pin(HAL_IN, "enable", &enable) ||
	    pin(HAL_IN, "is-homed", &is_homed) ||

	    pin(HAL_OUT, "debug_fb", &debug_fb) ||
	    pin(HAL_OUT, "debug_out", &debug_out) ||
	    pin(HAL_OUT, "debug_real", &debug_real) ||
	    pin(HAL_OUT, "offset", &offset) ||
	    pin(HAL_OUT, "position-out", &position_out) ||
	    pin(HAL_OUT, "index_count", &index_count) ||
	    pin(HAL_OUT, "home", &home) ||
	    0;
    }

    auto compute(long period) {
	double SCALE=*scale;
	int32_t pos_fb_i=int32_t(*pos_fb*SCALE);
	*debug_fb=pos_fb_i;

	int32_t cpi=*counts_per_index;
	if(!cpi)
	    return;
	int32_t diff=position_d1%cpi-pos_fb_i%cpi;
	if(diff>cpi/2)
	    diff-=cpi;
	else if(diff<-cpi/2)
	    diff+=cpi;
	*debug_real=pos_fb_i+diff;

	if(*index && count<10)
	    ++count;
	*home=!*is_homed && *enable && count>9 && timer==0 && !was_homed;
	if(*is_homed)
	    was_homed=1;
	*index_count=count;

	if(timer>0) {
	    timer--;
	    *offset=diff/SCALE;
	    correction=diff-position_d1;
	    *debug_out=correction;
	    *position_out=0;
	} else if(count<=1) {
	    /*  position_in is not yet reset */
	    *position_out=*position_in;
	} else if(count<=2) {
	    /* The LSB of position_in are now correct */
	    *position_out=position_d1+*position_in;
	} else if(count==3) {
	    *offset=diff/SCALE;
	    correction=diff+position_d2;
	    *debug_out=position_d1;
	    *position_out=*position_in+correction;
	} else
	    *position_out=*position_in+correction;
	position_d2=position_d1;
	position_d1=*position_in;
    }
};


std::vector<absolute_index_t*> state;

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
    comp_id = hal_init("absolute_index");
    if(comp_id < 0)
	return comp_id;

    for(int i=0; !r && i<count; i++) {
        auto p=reinterpret_cast<absolute_index_t*>(
	    hal_malloc(sizeof(absolute_index_t)));
        r=p->init(i);
	state.push_back(p);
    }

    if(!r)
	r = hal_export_funct("absolute_index.compute", compute, NULL,
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
