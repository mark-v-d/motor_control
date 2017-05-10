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

static int comp_id;

#ifdef MODULE_INFO
MODULE_INFO(linuxcnc, const_cast<char*>("component:etherdrive:"));
MODULE_INFO(linuxcnc, const_cast<char*>("pin:out:float:0:out::None:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("param:value:float:0:r::1.0:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("funct:send_sync:1:"));
MODULE_INFO(linuxcnc, const_cast<char*>("license:GPL"));
MODULE_LICENSE("GPL");
#endif // MODULE_INFO

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

static int udp;	// UDP socket

static struct sockaddr_in sync_addr;

/*******************************************************************************
    Packets
*******************************************************************************/
struct sync_t {
    uint32_t tx_sec;
    uint32_t tx_nsec;
    uint32_t rx_sec;
    uint32_t rx_nsec;
    uint32_t timer; 
    float integrator;
};

#include "drive_packet.h"

/******************************************************************************/
struct comp_state {
    struct comp_state *next;

    // input pins
    hal_float_t *Iset[2];
    hal_float_t *kP[2];
    hal_float_t *kI[2];
    hal_float_t *lim;

    // output pins
    hal_s32_t *counter;
    hal_float_t *adc[4];
    hal_float_t *position;
    hal_float_t *position2;
    hal_float_t *index2;
    hal_float_t *Irotor[2];
    hal_float_t *Vrotor[2];
    hal_float_t *I[2];
    hal_float_t *angle;
    hal_float_t *output[3];
    hal_float_t *vservo;

    hal_float_t *scale;
    hal_float_t *scale2;

    // hal_float_t value;
    struct sockaddr_in addr;
};
struct comp_state *comp_first_inst=0, *comp_last_inst=0;

static inline void update_output_pins(struct comp_state *dest, 
    struct output_t *pkt)
{
    *(dest->counter)=pkt->counter;
    *(dest->position)=pkt->position**(dest->scale);
    *(dest->position2)=pkt->position2**(dest->scale2);
    *(dest->index2)=pkt->index2**(dest->scale2);
    *(dest->angle)=pkt->angle;
    *(dest->vservo)=pkt->vservo;

    int i;
    for(i=0;i<4;i++)
	*(dest->adc[i])=pkt->adc[i];
    for(i=0;i<3;i++)
	*(dest->output[i])=pkt->output[i];
    for(i=0;i<2;i++) {
	*(dest->Irotor[i])=pkt->Irotor[i];
	*(dest->Vrotor[i])=pkt->Vrotor[i];
	*(dest->I[i])=pkt->I[i];
    }
}

static void send_sync(void *p, long period)
{
    auto task=reinterpret_cast<uspace_xenomai::RtaiTask*>(
	pthread_getspecific(uspace_xenomai::key));

    struct timespec now;
    struct itimerspec nxt;
    struct {
	uint32_t now_sec;
	uint32_t now_nsec;
	uint32_t next_sec;
	uint32_t next_nsec;
    } n;
    clock_gettime(CLOCK_MONOTONIC,&now);
    timer_gettime(task->timer, &nxt);
    n.next_sec=n.now_sec=now.tv_sec;
    n.next_nsec=n.now_nsec=now.tv_nsec;
    n.next_nsec+=nxt.it_value.tv_nsec;
    if(n.next_nsec>=1'000'000'000) {
	n.next_sec++;
	n.next_nsec-=1'000'000'000;
    }
    sendto(udp,&n,sizeof(n),0,reinterpret_cast<sockaddr*>(&sync_addr),
	sizeof(sync_addr));

    int r;
    do {
	uint8_t buffer[1024];
	struct sockaddr_in src;
	socklen_t src_len;
	r=recvfrom(udp, buffer, sizeof(buffer),
	    MSG_DONTWAIT,
	    reinterpret_cast<sockaddr*>(&src), &src_len);
	auto *p=reinterpret_cast<struct sync_t*>(buffer);
	struct comp_state *lp;
	for(lp=comp_first_inst; lp;lp=lp->next) {
	    if(src.sin_family!=AF_INET)
		continue;
	    if(!memcmp(&src.sin_addr,&lp->addr.sin_addr,
		sizeof(lp->addr.sin_addr))
	    ) { 
		switch(ntohs(src.sin_port)) {
		case 1: 
		    update_output_pins(lp,
			reinterpret_cast<struct output_t*>(buffer)); 
		    break;
		}
		break;
	    }
	}
    } while(r>=0);
}

static void send_control(void *p, long period)
{
    struct comp_state *lp;
    for(lp=comp_first_inst; lp;lp=lp->next) {
	struct input_t in;
	in.Iset[0]=*(lp->Iset[0]);
	in.Iset[1]=*(lp->Iset[1]);
	in.kP[0]=*(lp->kP[0]);
	in.kP[1]=*(lp->kP[1]);
	in.kI[0]=*(lp->kI[0]);
	in.kI[1]=*(lp->kI[1]);
	in.lim=*(lp->lim);
	sendto(udp,&in,sizeof(in),0,
	    reinterpret_cast<sockaddr*>(&lp->addr),sizeof(lp->addr));
    }
}

static int default_count=1, count=0;
static char *names[16] = {0,};
static char *ipaddr[16]= {0,};
static char const *bcast="192.168.0.255";

RTAPI_MP_INT(count, "number of etherdrive");
RTAPI_MP_STRING(bcast, "broadcast address");
RTAPI_MP_ARRAY_STRING(names, 16, "names of etherdrive");
RTAPI_MP_ARRAY_STRING(ipaddr, 16, "ip address of drives");


#define FP_IN(n,name) \
    r = hal_pin_float_newf(HAL_IN, &(inst->n), comp_id, "%s." #name, prefix); \
    if(r) \
	return r;
#define S32_IN(n,name) \
    r = hal_pin_s32_newf(HAL_IN, &(inst->n), comp_id, "%s." #name, prefix); \
    if(r) \
	return r;
#define FP_OUT(n,name) \
    r = hal_pin_float_newf(HAL_OUT, &(inst->n), comp_id, "%s." #name, prefix); \
    if(r) \
	return r;
#define S32_OUT(n,name) \
    r = hal_pin_s32_newf(HAL_OUT, &(inst->n), comp_id, "%s." #name, prefix); \
    if(r) \
	return r;

static int export_pins(char *prefix, long index) 
{
    char buf[HAL_NAME_LEN + 1];
    int r = 0;
    int sz = sizeof(struct comp_state);
    struct comp_state *inst = reinterpret_cast<comp_state*>(hal_malloc(sz));
    memset(inst, 0, sz);

    FP_IN(Iset[0],Iset-0);
    FP_IN(Iset[1],Iset-1);
    FP_IN(kP[0],kP-0);
    FP_IN(kP[1],kP-1);
    FP_IN(kI[0],kI-0);
    FP_IN(kI[1],kI-1);
    FP_IN(lim,lim);

    S32_OUT(counter,counter);
    FP_OUT(position,position);
    FP_OUT(adc[0],adc-0);
    FP_OUT(adc[1],adc-1);
    FP_OUT(adc[2],adc-2);
    FP_OUT(adc[3],adc-3);
    FP_OUT(Irotor[0],Irotor-0);
    FP_OUT(Irotor[1],Irotor-1);
    FP_OUT(Vrotor[0],Vrotor-0);
    FP_OUT(Vrotor[1],Vrotor-1);
    FP_OUT(I[0],I-0);
    FP_OUT(I[1],I-1);
    FP_OUT(angle,angle);
    FP_OUT(output[0],output-0);
    FP_OUT(output[1],output-1);
    FP_OUT(output[2],output-2);
    FP_OUT(vservo,vservo);
    FP_OUT(position2,position2);
    FP_OUT(index2,index2);

    // Local settings
    FP_IN(scale,scale);
    FP_IN(scale2,scale2);

#if 0
    r=hal_param_float_newf(HAL_RO, &(inst->value), comp_id, "%s.value", prefix);
    inst->value = 1.0;
    if(r) 
	return r;
#endif

    inst->addr.sin_family=AF_INET;
    inst->addr.sin_port=htons(1);
    struct hostent *hp=gethostbyname(ipaddr[index]);
    memcpy((void*) &inst->addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    if(comp_last_inst) 
	comp_last_inst->next = inst;
    comp_last_inst = inst;
    if(!comp_first_inst) 
	comp_first_inst = inst;
    return 0;
}

extern "C" int rtapi_app_main(void) 
{
    int r = 0;
    int i;
    comp_id = hal_init("etherdrive");
    if(comp_id < 0) 
	return comp_id;
    if(count && names[0]) {
        rtapi_print_msg(RTAPI_MSG_ERR,
	    "count= and names= are mutually exclusive\n");
        return -EINVAL;
    }
    if(!count && !names[0]) 
	count = default_count;
    if(count) {
        for(i=0; i<count; i++) {
            char buf[HAL_NAME_LEN + 1];
            rtapi_snprintf(buf, sizeof(buf), "etherdrive.%d", i);
            r = export_pins(buf, i);
            if(r != 0) break;
       }
    } else {
        int max_names = sizeof(names)/sizeof(names[0]);
        for(i=0; (i < max_names) && names[i]; i++) {
            if (strlen(names[i]) < 1) {
                rtapi_print_msg(RTAPI_MSG_ERR, 
		    "names[%d] is invalid (empty string)\n", i);
                r = -EINVAL;
                break;
            }
            r = export_pins(names[i], i);
            if(r != 0) break;
       }
    }

    // Initialise socket and destination address for sync
    udp=socket(AF_INET, SOCK_DGRAM, 0);
    sync_addr.sin_family=AF_INET;
    sync_addr.sin_port=htons(3);
    struct hostent *hp=gethostbyname(bcast);
    memcpy((void*) &sync_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    // Export the send-sync function
    if(!r) 
	r = hal_export_funct("etherdrive.sync", 
	    send_sync, 
	    NULL, 1, 0, comp_id);
    if(!r) 
	r = hal_export_funct("etherdrive.control", 
	    send_control, 
	    NULL, 1, 0, comp_id);

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
