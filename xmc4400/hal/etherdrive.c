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

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

struct comp_state {
    struct comp_state *next;
    hal_float_t *out;
    hal_float_t value;
};
struct comp_state *comp_first_inst=0, *comp_last_inst=0;


static int udp;	// UDP socket

static struct sockaddr_in sync_addr;

struct sync_t {
    uint32_t tx_sec;
    uint32_t tx_nsec;
    uint32_t rx_sec;
    uint32_t rx_nsec;
    uint32_t timer; 
    float integrator;
};

static void send_sync(void *p, long period)
{
    int r;
    do {
	uint8_t buffer[1024];
	struct sockaddr src;
	socklen_t src_len;
	r=recvfrom(udp, buffer, sizeof(buffer),
	    MSG_DONTWAIT,
	    &src, &src_len);
    } while(r>=0);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC,&now);
    struct {
	uint32_t now_sec;
	uint32_t now_nsec;
	uint32_t next_sec;
	uint32_t next_nsec;
    } n;
    n.next_sec=n.now_sec=now.tv_sec;
    n.next_nsec=n.now_nsec=now.tv_nsec;
    n.next_nsec+=period;
    if(n.next_nsec>1000000000) {
	n.next_sec++;
	n.next_nsec-=1000000000;
    }
    sendto(udp,&n,sizeof(n),0,&sync_addr,sizeof(sync_addr));
}

static int export(char *prefix, long extra_arg) 
{
    char buf[HAL_NAME_LEN + 1];
    int r = 0;
    int sz = sizeof(struct comp_state);
    struct comp_state *inst = hal_malloc(sz);
    memset(inst, 0, sz);
    r = hal_pin_float_newf(HAL_OUT, &(inst->out), comp_id,
        "%s.out", prefix);
    if(r != 0) return r;
    r = hal_param_float_newf(HAL_RO, &(inst->value), comp_id,
        "%s.value", prefix);
    inst->value = 1.0;
    if(r != 0) return r;

    if(comp_last_inst) comp_last_inst->next = inst;
    comp_last_inst = inst;
    if(!comp_first_inst) comp_first_inst = inst;
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

    // Initialise socket and destination address for sync
    udp=socket(AF_INET, SOCK_DGRAM, 0);
    sync_addr.sin_family=AF_INET;
    sync_addr.sin_port=htons(3);
    struct hostent *hp=gethostbyname("192.168.0.5");
    memcpy((void*) &sync_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    // Export the send-sync function
    if(!r) 
	r = hal_export_funct("etherdrive.send-sync", 
	    send_sync, 
	    NULL, 1, 0, comp_id);

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
