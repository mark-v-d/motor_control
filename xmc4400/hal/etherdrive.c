#include "rtapi.h"
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "rtapi_string.h"
#include "rtapi_errno.h"
#include "hal.h"
#include "rtapi_math64.h"

static int comp_id;

#ifdef MODULE_INFO
MODULE_INFO(linuxcnc, "component:etherdrive:");
MODULE_INFO(linuxcnc, "pin:out:float:0:out::None:None");
MODULE_INFO(linuxcnc, "param:value:float:0:r::1.0:None");
MODULE_INFO(linuxcnc, "funct:send_sync:1:");
MODULE_INFO(linuxcnc, "license:GPL");
MODULE_LICENSE("GPL");
#endif // MODULE_INFO

struct __comp_state {
    struct __comp_state *_next;
    hal_float_t *out;
    hal_float_t value;
};
struct __comp_state *__comp_first_inst=0, *__comp_last_inst=0;

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

int udp;	// UDP socket

struct sockaddr_in sync_addr;

struct sync_t {
    uint32_t tx_sec;
    uint32_t tx_nsec;
    uint32_t rx_sec;
    uint32_t rx_nsec;
    uint32_t timer; 
    float integrator;

    /*
    sync_t(void) {}
    sync_t(uint8_t *p) {
	sync_t *src=reinterpret_cast<sync_t*>(p);
	*this=*src;
    }
    */
};

static void send_sync(struct __comp_state *__comp_inst, long period)
{
/*
	struct itimerspec nxt;
	timer_gettime(tid, &nxt);
	n.next_sec=now.tv_sec;
	n.next_nsec=now.tv_nsec+nxt.it_value.tv_nsec;
	if(n.next_nsec>1'000'000'000) {
	    n.next_sec++;
	    n.next_nsec-=1'000'000'000;
	}
*/
	if(udp>0) {
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC,&now);
		struct {
		    uint32_t now_sec;
		    uint32_t now_nsec;
		    uint32_t next_sec;
		    uint32_t next_nsec;
		} n;
		n.now_sec=now.tv_sec;
		n.now_nsec=now.tv_nsec;
		sendto(udp,&n,sizeof(n),0,&sync_addr,sizeof(sync_addr));
	} else {
		printf("Etherdrive init\n");
		udp=socket(AF_INET, SOCK_DGRAM, 0);
		sync_addr.sin_family=AF_INET;
		sync_addr.sin_port=htons(3);
		struct hostent *hp=gethostbyname("192.168.0.5");
		memcpy((void*) &sync_addr.sin_addr, hp->h_addr_list[0], 
			hp->h_length);
	}

}

static int export(char *prefix, long extra_arg) 
{
    char buf[HAL_NAME_LEN + 1];
    int r = 0;
    int sz = sizeof(struct __comp_state);
    struct __comp_state *inst = hal_malloc(sz);
    memset(inst, 0, sz);
    r = hal_pin_float_newf(HAL_OUT, &(inst->out), comp_id,
        "%s.out", prefix);
    if(r != 0) return r;
    r = hal_param_float_newf(HAL_RO, &(inst->value), comp_id,
        "%s.value", prefix);
    inst->value = 1.0;
    if(r != 0) return r;

    rtapi_snprintf(buf, sizeof(buf), "%s.send-sync", prefix);
    r = hal_export_funct(buf, (void(*)(void *inst, long))send_sync, 
	inst, 1, 0, comp_id);

    if(r != 0) return r;
    if(__comp_last_inst) __comp_last_inst->_next = inst;
    __comp_last_inst = inst;
    if(!__comp_first_inst) __comp_first_inst = inst;
    return 0;
}

static int default_count=1, count=0;
char *names[16] = {0,};

RTAPI_MP_INT(count, "number of etherdrive");
RTAPI_MP_ARRAY_STRING(names, 16, "names of etherdrive");

int rtapi_app_main(void) 
{
    int r = 0;
    int i;
    comp_id = hal_init("etherdrive");
    if(comp_id < 0) return comp_id;
    if(count && names[0]) {
        rtapi_print_msg(RTAPI_MSG_ERR,
	    "count= and names= are mutually exclusive\n");
        return -EINVAL;
    }
    if(!count && !names[0]) count = default_count;
    if(count) {
        for(i=0; i<count; i++) {
            char buf[HAL_NAME_LEN + 1];
            rtapi_snprintf(buf, sizeof(buf), "etherdrive.%d", i);
            r = export(buf, i);
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
            r = export(names[i], i);
            if(r != 0) break;
       }
    }
    if(r) {
        hal_exit(comp_id);
    } else {
        hal_ready(comp_id);
    }
    return r;
}

void rtapi_app_exit(void) 
{
    hal_exit(comp_id);
}
