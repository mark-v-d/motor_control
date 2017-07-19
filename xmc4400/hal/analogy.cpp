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

static int comp_id;
static a4l_desc_t dsc; 

static hal_bit_t ***unit;
int *bits;

static void an_sync(void *p, long period)
{
    int input_idx=0, output_idx=0, bidir_idx=0;
    for(int u=0;u<dsc.nb_subd;u++) {
	a4l_sbinfo_t *sbinfo;
	int err=a4l_get_subdinfo(&dsc, u, &sbinfo);
	if(err<0) {
	    rtapi_print_msg(RTAPI_MSG_ERR,"a4l_get_subdinfo failed\n");
	    return;
	}
	if(sbinfo->flags==A4L_SUBD_DI) {
	    int mask, value;
	    err=a4l_sync_dio(&dsc, u, &mask, &value);
	    for(int bit=0; 
		bit<sbinfo->nb_chan && bit<bits[u];
		bit++, input_idx++
	    ) {
		*(unit[u][bit])=value&1;
		value>>=1;
	    }
	} else if(sbinfo->flags==A4L_SUBD_DO) {
	    int mask=(1<<sbinfo->nb_chan)-1;
	    int value=0;
	    for(int bit=0; 
		bit<sbinfo->nb_chan && bit<bits[u];
		bit++
	    ) {
		if(*(unit[u][bit]))
		    value|=1<<bit;
	    }
	    err=a4l_sync_dio(&dsc, u, &mask, &value);
	} else if(sbinfo->flags==A4L_SUBD_DIO) {
	    int mask=(1<<sbinfo->nb_chan)-1;
	    int value=0;
	    for(int bit=0; 
		bit<sbinfo->nb_chan && bit<bits[u];
		bit++
	    ) {
		if(*(unit[u][sbinfo->nb_chan+bit]))
		    value|=1<<bit;
	    }
	    err=a4l_sync_dio(&dsc, u, &mask, &value);
	    for(int bit=0; 
		bit<sbinfo->nb_chan && bit<bits[u];
		bit++
	    ) {
		*(unit[u][bit])=value&1;
		value>>=1;
	    }
	} else {
	    rtapi_print_msg(RTAPI_MSG_WARN,
		"unit %d ignored, flags=0x%08x\n",
		u, sbinfo->flags);
	}
    }
}

extern "C" int rtapi_app_main(void) 
{
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

	unit=reinterpret_cast<hal_bit_t***>(
	    hal_malloc(dsc.nb_subd*sizeof(hal_bit_t**)));
	bits=reinterpret_cast<int*>(
	    hal_malloc(dsc.nb_subd*sizeof(int)));
	for(int u=0;u<dsc.nb_subd;u++) {
	    a4l_sbinfo_t *sbinfo;
	    err = a4l_get_subdinfo(&dsc, u, &sbinfo);
	    if(err<0)
		throw("a4l_get_subdinfo");
	    if(sbinfo->flags==A4L_SUBD_DI) {
		bits[u]=sbinfo->nb_chan;
		unit[u]=reinterpret_cast<hal_bit_t**>(
		    hal_malloc(bits[u]*sizeof(hal_bit_t *)));
		for(int i=0;i<bits[u];i++) {
		    hal_pin_bit_newf(HAL_OUT, &(unit[u][i]), comp_id,
			"analogy.%d.in-%d", u, i);
		} 
	    } else if(sbinfo->flags==A4L_SUBD_DO) {
		bits[u]=sbinfo->nb_chan;
		unit[u]=reinterpret_cast<hal_bit_t**>(
		    hal_malloc(bits[u]*sizeof(hal_bit_t *)));
		for(int i=0;i<bits[u];i++) {
		    hal_pin_bit_newf(HAL_IN, &(unit[u][i]), comp_id,
			"analogy.%d.out-%d", u, i);
		} 
		int mask=0, value;
		err=a4l_sync_dio(&dsc, u, &mask, &value);
		for(int bit=0; 
		    bit<sbinfo->nb_chan && bit<bits[u];
		    bit++
		) {
		    *(unit[u][bit])=value&1;
		    value>>=1;
		}
	    } else if(sbinfo->flags==A4L_SUBD_DIO) {
		bits[u]=sbinfo->nb_chan;
		unit[u]=reinterpret_cast<hal_bit_t**>(
		    hal_malloc(2*bits[u]*sizeof(hal_bit_t *)));
		for(int i=0;i<bits[u];i++) {
		    hal_pin_bit_newf(HAL_OUT, &(unit[u][i]), comp_id,
			"analogy.%d.in-%d", u, i);
		    hal_pin_bit_newf(HAL_IN, &(unit[u][sbinfo->nb_chan+i]), 
			comp_id, "analogy.%d.out-%d", u, i);
		    *unit[u][sbinfo->nb_chan+i]=1;
		} 
	    } else {
		rtapi_print_msg(RTAPI_MSG_WARN,
		    "unit %d ignored, flags=0x%08x\n",
		    u, sbinfo->flags);
	    }
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
