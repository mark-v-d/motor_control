/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include "raw_socket.h"

#include <sys/io.h>

#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#include "packet.h"

#include <iostream>

constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};
constexpr uint16_t sync_port=3;

struct __attribute__ ((__packed__)) sync_send_t:public udp_t {
    uint32_t now_sec;
    uint32_t now_nsec;
    uint32_t next_sec;
    uint32_t next_nsec;

    template <class T>
    sync_send_t(struct timespec now, timespec next, T const &skt) {
	now_sec=now.tv_sec;
	now_nsec=now.tv_nsec;
	next_sec=next.tv_sec;
	next_nsec=next.tv_nsec;

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

struct sync_t {
    struct timespec timestamp;
    int rx_size;
    struct sync_response {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	int32_t timer;
	float integrator;
    } data;
};

std::ostream &operator<<(std::ostream &s, sync_t d) {
    s << d.timestamp.tv_sec << " " << d.timestamp.tv_nsec << " " << d.rx_size
	<< " " << d.data.tx_seconds << " " << d.data.tx_nanoseconds
	<< " " << d.data.rx_seconds << " " << d.data.rx_nanoseconds
	<< " " << d.data.timer << " " << d.data.integrator
	;
    return s;
}

struct  __attribute__ ((__packed__)) sync_recv_t:public udp_t {
    ::sync_t::sync_response data;
};


raw_socket skt("eth2");
std::array<sync_t,10*4500> table;

void *rt_thread(void *data)
{
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
	sync_recv_t sync;
	char txt[1024];
    } buffer;
    for(auto &x:table) {
	struct timespec timestamp;
	if(clock_gettime(CLOCK_MONOTONIC, &x.timestamp)) {
	    perror("clock_gettime");
	    return NULL;
	}

	ts.tv_nsec+=222222;
	while(ts.tv_nsec>=1'000'000'000) {
	    ts.tv_sec++;
	    ts.tv_nsec-=1'000'000'000;
	}

	sync_send_t pkt(x.timestamp, ts, skt);
	skt.send(pkt);

	outb(0,base);
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
	outb(0xff,base);

	do{
	    rx_size=recvfrom(skt.socket(),
		&buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL);
	    if(rx_size==sizeof(buffer.sync))
		x.data=buffer.sync.data;
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
