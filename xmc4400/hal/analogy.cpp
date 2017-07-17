#include "rtapi.h"
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "rtapi_string.h"
#include "rtapi_errno.h"
#include "hal.h"
#include "rtapi_math64.h"
#include "../src/rtapi/rtapi_uspace.hh"
#include "../src/rtapi/uspace_xenomai.hh"

#include <stdlib.h>
#include <pthread.h>
#include <rtdm/analogy.h>

#include <vector>

static int comp_id;
static a4l_desc_t dsc; 

int inputs=0, outputs=0, bidirs=0;
hal_bit_t **input, **output, **bidir;


static void an_sync(void *p, long period)
{
    int input_idx=0, output_idx=0, bidir_idx=0;
    for(int i=0;i<dsc.nb_subd;i++) {
	a4l_sbinfo_t *sbinfo;
	int err=a4l_get_subdinfo(&dsc, i, &sbinfo);
	if(err<0) {
	    rtapi_print_msg(RTAPI_MSG_ERR,"a4l_get_subdinfo failed\n");
	    return;
	}
	if(sbinfo->flags==A4L_SUBD_DI) {
	    int mask, value;
	    err=a4l_sync_dio(&dsc, i, &mask, &value);
	    for(int bit=0; 
		bit<sbinfo->nb_chan && input_idx<inputs; 
		bit++, input_idx++
	    ) {
		*(input[input_idx])=value&1;
		value>>=1;
	    }
	} else if(sbinfo->flags==A4L_SUBD_DO) {
	} else if(sbinfo->flags==A4L_SUBD_DIO) {
	} else {
	    rtapi_print_msg(RTAPI_MSG_WARN,
		"unit %d ignored, flags=0x%08x\n",
		i, sbinfo->flags);
	}
    }
}

extern "C" int rtapi_app_main(void) 
{
    int r = 0;
    int i;
    comp_id = hal_init("analogy");
    if(comp_id < 0) 
	return comp_id;

    int err;
    try {
	err=a4l_open(&dsc, "analogy0");
	if(err<0)
	    throw("a4l_open");
	dsc.sbdata=malloc(dsc.sbsize);
	err=a4l_fill_desc(&dsc); 
	if(err<0) 
	    throw("a4l_fill_desc");

	for(int i=0;i<dsc.nb_subd;i++) {
	    a4l_sbinfo_t *sbinfo;
	    err = a4l_get_subdinfo(&dsc, i, &sbinfo);
	    if(err<0)
		throw("a4l_get_subdinfo");
	    if(sbinfo->flags==A4L_SUBD_DI) {
		inputs+=sbinfo->nb_chan;
	    } else if(sbinfo->flags==A4L_SUBD_DO) {
		outputs+=sbinfo->nb_chan;
	    } else if(sbinfo->flags==A4L_SUBD_DIO) {
		bidirs+=sbinfo->nb_chan;
	    } else {
		rtapi_print_msg(RTAPI_MSG_WARN,
		    "unit %d ignored, flags=0x%08x\n",
		    i, sbinfo->flags);
	    }
	}
	input=reinterpret_cast<hal_bit_t**>(
	    hal_malloc(inputs*sizeof(hal_bit_t *)));
	for(int i=0;i<inputs;i++) {
	    hal_pin_bit_newf(HAL_OUT, &(input[i]), comp_id,
		"analogy.in-%d", i);
	} 

	output=reinterpret_cast<hal_bit_t**>(
	    hal_malloc(outputs*sizeof(hal_bit_t *)));
	for(int i=0;i<outputs;i++) {
	    hal_pin_bit_newf(HAL_IN, &(output[i]), comp_id,
		"analogy.out-%d", i);
	} 

	bidir=reinterpret_cast<hal_bit_t**>(
	    hal_malloc(bidirs*sizeof(hal_bit_t *)));
	for(int i=0;i<bidirs;i++) {
	    hal_pin_bit_newf(HAL_IO, &(bidir[i]), comp_id,
		"analogy.io-%d", i);
	} 

	err=hal_export_funct("analogy.sync", an_sync, NULL, 1, 0, comp_id);
	if(err)
	    throw("hal_export_funct");
    } catch(char const *msg) {
	rtapi_print_msg(RTAPI_MSG_ERR,"%s failed\n",msg);
        hal_exit(comp_id);
        return err;
    } catch(...) {
	rtapi_print_msg(RTAPI_MSG_ERR,"Oops unhandled exception\n");
        hal_exit(comp_id);
        return err;
    }
    hal_ready(comp_id);
    return err;
}

extern "C" void rtapi_app_exit(void) 
{
    hal_exit(comp_id);
}
