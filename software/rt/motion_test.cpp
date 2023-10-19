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

#include "drive_packet.h"

#include <iostream>

constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};
constexpr uint16_t sync_port=3;

struct __attribute__ ((__packed__)) sync_send_t:public udp_t, public sync_ns::to_drive {
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

struct sync_t:public sync_ns::to_host {
    timespec timestamp;
    size_t rx_size;
};

struct sync_recv_t:public udp_t, public sync_ns::to_host {
};

std::ostream &operator<<(std::ostream &s, sync_t d) {
    s << d.timestamp.tv_sec << " " << d.timestamp.tv_nsec << " " << d.rx_size
	<< " " << d.tx_seconds << " " << d.tx_nanoseconds
	<< " " << d.rx_seconds << " " << d.rx_nanoseconds
	<< " " << d.timer << " " << d.integrator
	;
    return s;
}

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
	udp_t udp;
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
	    if(buffer.udp.src_port==htons(3) && rx_size==sizeof(buffer.sync)) {
		x=static_cast<decltype(x)>(buffer.sync);
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
