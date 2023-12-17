#include "rtapi.h"
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "hal.h"

static int comp_id;

#ifdef MODULE_INFO
MODULE_INFO(linuxcnc, const_cast<char*>("component:etherdrive:"));
//MODULE_INFO(linuxcnc, const_cast<char*>("pin:out:float:0:out::None:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("param:value:float:0:r::1.0:None"));
MODULE_INFO(linuxcnc, const_cast<char*>("funct:sync:1:"));
MODULE_INFO(linuxcnc, const_cast<char*>("license:GPL"));
MODULE_LICENSE("GPL");
#endif // MODULE_INFO

#include "raw_socket.h"
#include "drive_io.h"

constexpr std::array<uint8_t,6> bcast_mac{255,255,255,255,255,255};
constexpr std::array<uint8_t,4> bcast_ip{255,255,255,255};

std::array<std::array<uint8_t,6>,16> mac;

raw_socket skt;

/******************************************************************************/
struct comp_state {
    // input pins
    hal_float_t *Iset[2];
    hal_bit_t *enable;

    // output pins
    hal_float_t *position;
    hal_float_t *position2;
    hal_float_t *index2;
    hal_float_t *Irotor[2];
    hal_float_t *Vrotor[2];
    hal_float_t *angle;
    hal_float_t *Vservo;
    hal_u32_t *invalid;

    // parameters
    hal_float_t *scale[2];

    void init(int i) {
	std::string prefix="etherdrive."+std::to_string(i)+".";
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
	int ok=
	    !pin(HAL_IN, "Iset-0", &Iset[0]) &&
	    !pin(HAL_IN, "Iset-1", &Iset[1]) &&
	    !pin(HAL_IN, "enable", &enable) &&
	    !pin(HAL_IN, "scale", &scale[0]) &&
	    !pin(HAL_IN, "scale2", &scale[1]) &&
	    !pin(HAL_OUT,"position", &position) &&
	    !pin(HAL_OUT,"position2", &position2) &&
	    !pin(HAL_OUT,"index2", &index2) &&
	    !pin(HAL_OUT,"Irotor-0", &Irotor[0]) &&
	    !pin(HAL_OUT,"Irotor-1", &Irotor[1]) &&
	    !pin(HAL_OUT,"Vrotor-0", &Vrotor[0]) &&
	    !pin(HAL_OUT,"Vrotor-1", &Vrotor[1]) &&
	    !pin(HAL_OUT,"angle", &angle) &&
	    !pin(HAL_OUT,"Vservo", &Vservo) &&
	    !pin(HAL_OUT,"invalid", &invalid)
	    ;
    }

    int offset[2];

    void update(motion_ns::to_host &buffer){
	if(offset[0]==0 && offset[1]==0) {
	    offset[0]=buffer.position;
	    offset[1]=buffer.position2;
	}
	*position=(buffer.position-offset[0]) * *scale[0];
	*position2=(buffer.position2-offset[1]) * *scale[1];
	*index2=buffer.index2 * *scale[1];
	*Irotor[0]=buffer.Irotor[0];
	*Irotor[1]=buffer.Irotor[1];
	*Vrotor[0]=buffer.Vrotor[0];
	*Vrotor[1]=buffer.Vrotor[1];
	*angle=buffer.angle;
	*Vservo=buffer.Vservo;
	*invalid+=buffer.valid? 0:1;
    }
};

std::vector<comp_state*> state;

static void send(void *p, long period)
{
    for(int i=0; i<state.size(); i++) {
	if(!*state[i]->enable)
	    continue;
	std::array<uint8_t,4> ip{192,168,0,uint8_t(i+1)};
	std::complex<float> I{
	    float(*state[i]->Iset[0]),
	    float(*state[i]->Iset[1])
	};
	skt.send(motion_ns::send_t(skt, mac[i], ip, I));
    }
}

static void sync(void *p, long period)
{
    struct timespec timestamp;
    if(clock_gettime(CLOCK_MONOTONIC, &timestamp)) {
	perror("clock_gettime");
    }

    auto ts=timestamp;
    ts.tv_nsec+=period;
    while(ts.tv_nsec>=1'000'000'000) {
	ts.tv_sec++;
	ts.tv_nsec-=1'000'000'000;
    }

    skt.send(sync_ns::send_t(skt,bcast_mac,bcast_ip,timestamp,ts));

    ssize_t rx_size;
    do{
	union rx_types {
	    udp_t udp;
	    sync_ns::recv_t sync;
	    motion_ns::recv_t motion;
	    char txt[1024];
	} buffer;
	rx_size=recvfrom(skt.socket(),
	    &buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL);
	unsigned index=buffer.udp.src_ip[3]-1;
	if(buffer.udp.src_port==htons(sync_ns::port)
	    && rx_size==sizeof(buffer.sync)
	) {
	    //static_cast<sync_ns::to_host&>(x)=buffer.sync;
	} else if(buffer.udp.src_port==htons(motion_ns::port)
	    && rx_size==sizeof(buffer.motion) && index<state.size()
	) {
	    state[index]->update(buffer.motion);
	}
    } while(rx_size>0);
}


////////////////////////////////////////////////////////////////////////////////
static char *names[16] = {0,};
static char const *device="eth1";

RTAPI_MP_STRING(device, "ethernet device");
RTAPI_MP_ARRAY_STRING(names, 16, "mac addresses of etherdrives");

extern "C" int rtapi_app_main(void)
{
    int r = 0;
    int i;
    comp_id = hal_init("etherdrive");
    if(comp_id < 0)
	return comp_id;

    for(int i=0; names[i] && i<16; i++) {
	std::array<unsigned,6> d;
	char na[100];
	strcpy(na,names[i]);
	if(sscanf(na,"%x:%x:%x:%x:%x:%x:%x",
	    &d[0],&d[1],&d[2],&d[3],&d[4],&d[5])!=6
	) {
	    rtapi_print_msg(RTAPI_MSG_ERR, "invalid mac address: %s",names[i]);
	    return -EINVAL;
	};
	for(int j=0; j<d.size(); j++) {
	    if(d[j]>255) {
		rtapi_print_msg(RTAPI_MSG_ERR, "invalid mac address: %s",
		    names[i]);
		return -EINVAL;
	    }
	    mac[i][j]=d[j];
	}
        auto p=reinterpret_cast<comp_state*>(hal_malloc(sizeof(comp_state)));
        p->init(i);
	state.push_back(p);
    }

    try {
	skt.init(device);
    } catch (std::string const &s) {
        rtapi_print_msg(RTAPI_MSG_ERR, "Cannot open device");
        return -EINVAL;
    }

    // Export the send-sync function
    if(!r)
	r = hal_export_funct("etherdrive.sync", sync, NULL, 1, 0, comp_id);
    if(!r)
	r = hal_export_funct("etherdrive.send", send, NULL, 1, 0, comp_id);
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
