#include "raw_socket.h"

#include <sys/io.h>

#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace std { using fmt::format; }
#include <string>
#include <fstream>

#include "drive_packet.h"

#include <iostream>
#include <complex>
#include <chrono>
using namespace std::complex_literals;
using namespace std::chrono_literals;

//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x8d, 0x11, 0x11, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};
constexpr uint16_t sync_port=3;
constexpr uint16_t motion_port=2;

using timebase_t=std::chrono::duration<int,std::ratio<1,4500>>;

std::complex<double> current=0.1i;

struct __attribute__ ((__packed__)) sync_send_t:
    public udp_t, public sync_ns::to_drive
{
    template <class T>
    sync_send_t(struct timespec now, timespec next, T const &skt) {
	tx_seconds=now.tv_sec;
	tx_nanoseconds=now.tv_nsec;
	rx_seconds=next.tv_sec;
	rx_nanoseconds=next.tv_nsec;

	dst_mac=::dst_mac;
	dst_ip=::dst_ip;
	dst_port=htons(sync_port);

	src_mac=skt.src_mac;
	src_ip=skt.src_ip;
	type=htons(ETH_P_IP);

	src_port=htons(49895);
	id=0xb3a3;

	version_headerlength=0x45;	// IPv4
	services=0;
	checksum=0;
	flags_fragment_offset=0x40;	// Don't fragment
	ttl=64;
	protocol=ipv4_header_t::UDP;

	length=hton(sizeof(*this)-sizeof(ethernet_t));
	udp_length=hton(sizeof(*this)-sizeof(ipv4_t));
	do_ipv4_checksum();
    }
};

struct sync_recv_t:public udp_t, public sync_ns::to_host {
};

struct sync_t:public sync_ns::to_host, public motion_ns::to_host {
    timespec timestamp;
    std::complex<float> I;
    int32_t setpoint;
};

std::ostream &operator<<(std::ostream &s, sync_t d) {
    s << d.timestamp.tv_sec << " " << d.timestamp.tv_nsec	// 1,2
	<< " " << d.tx_seconds << " " << d.tx_nanoseconds	// 3,4
	<< " " << d.rx_seconds << " " << d.rx_nanoseconds	// 5,6
	<< " " << d.timer << " " << d.integrator		// 7,8
	<< " " << d.position	// 9
	<< " " << d.angle	// 10
	<< " " << d.valid	// 11
	<< " " << d.Irotor[0]	// 12
	<< " " << d.Irotor[1]	// 13
	<< " " << d.Vrotor[0]	// 14
	<< " " << d.Vrotor[1]	// 15
	<< " " << d.ADC[0]	// 16
	<< " " << d.ADC[1]	// 17
	<< " " << d.invalid 	// 18
	<< " " << d.timer_delta	// 19
	<< " " << d.rx_counter	// 20
	<< " " << d.rx_data[0]	// 21
	<< " " << d.rx_data[1]	// 22
	<< " " << d.rx_data[2]	// 23
	<< " " << d.rx_data[3]	// 24
	<< " " << d.glass_counter	// 25
	<< " " << d.glass_index		// 26
	<< " " << d.setpoint	// 27
	<< " " << real(d.I)	// 28
	<< " " << imag(d.I)	// 29
	;
    return s;
}

struct motion_recv_t:public udp_t, public motion_ns::to_host {
};

struct __attribute__ ((__packed__)) motion_send_t:
    public udp_t, public motion_ns::to_drive
{
    template <class T>
    motion_send_t(T const &skt,std::complex<float> I) {
	new_data++; new_data|=0x80000000;
	Iset[0]=I.real();
	Iset[1]=I.imag();

	dst_mac=::dst_mac;
	dst_ip=::dst_ip;
	dst_port=htons(motion_port);

	src_mac=skt.src_mac;
	src_ip=skt.src_ip;
	type=htons(ETH_P_IP);

	src_port=htons(49896);
	id=0xb3a3;

	version_headerlength=0x45;	// IPv4
	services=0;
	checksum=0;
	flags_fragment_offset=0x40;	// Don't fragment
	ttl=64;
	protocol=ipv4_header_t::UDP;

	length=hton(sizeof(*this)-sizeof(ethernet_t));
	udp_length=hton(sizeof(*this)-sizeof(ipv4_t));
	do_ipv4_checksum();
    }
};


std::vector<sync_t> table;

raw_socket skt("eth2");

void *rt_thread(void *data)
{
    timebase_t time=0s;
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts)) {
	perror("clock_gettime");
	return NULL;
    }
    uint16_t base=0xec00;

    if(ioperm(base, 8, 1)) {
	perror("request_region failed\n");
	return NULL;
    }

    ssize_t rx_size;
    union rx_types {
	udp_t udp;
	sync_recv_t sync;
	motion_recv_t motion;
	char txt[1024];
    } buffer;

    motion_ns::to_host motion;
    for(size_t i=0; i<table.size()-1; i++) {
	time++;
	struct timespec timestamp;
	if(clock_gettime(CLOCK_MONOTONIC, &table[i].timestamp)) {
	    perror("clock_gettime");
	    return NULL;
	}

	ts.tv_nsec+=222222;
	while(ts.tv_nsec>=1'000'000'000) {
	    ts.tv_sec++;
	    ts.tv_nsec-=1'000'000'000;
	}

	sync_send_t pkt(table[i].timestamp, ts, skt);
	skt.send(pkt);

	motion_send_t mp(skt,table[i].I);
	skt.send(mp);

	outb(0,base);
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
	outb(0xff,base);

	do{
	    rx_size=recvfrom(skt.socket(),
		&buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL);
	    if(buffer.udp.src_port==htons(sync_port)
		&& rx_size==sizeof(buffer.sync)
	    ) {
		static_cast<sync_ns::to_host&>(table[i+1])=
		    static_cast<sync_ns::to_host>(buffer.sync);
	    } else if(buffer.udp.src_port==htons(motion_port)
		&& rx_size==sizeof(buffer.motion)
	    ) {
		static_cast<motion_ns::to_host&>(table[i+1])=
		    static_cast<motion_ns::to_host>(buffer.motion);
	    }
	} while(rx_size>0);

    }
    return NULL;
}

int main(int argc, char *argv[])
{
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;
    int ret;

    if(argc<1)
	return 1;

    std::ifstream f(argv[1]);
    if(!f) {
	perror(argv[1]);
	return 1;
    }

    for(int i=0; i<0.1s/timebase_t(1); i++) // synchronisation for 0.1s
	table.emplace_back(sync_t{.I=std::complex<float>{0,0}});

    std::string s;
    while(std::getline(f,s)) {
	if(!s.size() || s[0]=='#')
	    continue;
	table.emplace_back(sync_t{.I=std::complex<float>{0,stod(s)}});
    }

    for(int i=0; i<1s/timebase_t(1); i++) // lead out of 1s
	table.emplace_back(sync_t{.I=std::complex<float>{0,0}});

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
	    printf("mlockall failed: %m\n");
	    exit(-2);
    }
    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
	    printf("init pthread attributes failed\n");
	    goto out;
    }
    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
	printf("pthread setstacksize failed\n");
	goto out;
    }
    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
	    printf("pthread setschedpolicy failed\n");
	    goto out;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
	    printf("pthread setschedparam failed\n");
	    goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
	    printf("pthread setinheritsched failed\n");
	    goto out;
    }
    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, rt_thread, NULL);
    if (ret) {
	    printf("create pthread failed\n");
	    goto out;
    }
    /* Join the thread and wait until it is done */
    ret = pthread_join(thread, NULL);
    if (ret)
	    printf("join pthread failed: %m\n");
    for(auto &x: table) {
	std::cout << x << std::endl;
    }
out:
    return ret;
}
