/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#include <sys/io.h>

#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>


#include <packet.h>

#include <iostream>

#define MY_DEST_MAC0	0xc2
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x86
#define MY_DEST_MAC3	0x05
#define MY_DEST_MAC4	0x10
#define MY_DEST_MAC5	0xc0

#define DEFAULT_IF	"eth1"
#define BUF_SIZ		1024
#define ETHER_TYPE	0x0800

struct sync_t {
    struct timespec timestamp;
    int dt;
    int rx_size;
    struct sync_response {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	uint32_t timer;
	float integrator;
    } data;
};

std::ostream &operator<<(std::ostream &s, sync_t d) {
    s << d.timestamp.tv_sec << " " << d.timestamp.tv_nsec << " " << d.rx_size
	<< " " << d.dt
	<< " " << d.data.tx_seconds << " " << d.data.tx_nanoseconds
	<< " " << d.data.rx_seconds << " " << d.data.rx_nanoseconds
	<< " " << d.data.timer << " " << d.data.integrator
	;
    return s;
}

std::array<sync_t,45000> table;

struct ifreq if_idx;
struct ifreq if_mac;
int sockfd, rxs;

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

    for(auto &x:table) {
	struct timespec timestamp;
	if(clock_gettime(CLOCK_MONOTONIC, &x.timestamp)) {
	    perror("clock_gettime");
	    return NULL;
	}
	outb(0xff,base);
	struct __attribute__ ((__packed__)) sync_t:public udp_t {
	    uint32_t now_sec;
	    uint32_t now_nsec;
	    uint32_t next_sec;
	    uint32_t next_nsec;
	} pkt;

	pkt.now_sec=x.timestamp.tv_sec;
	pkt.now_nsec=x.timestamp.tv_nsec;

	ts.tv_nsec+=222222;
	while(ts.tv_nsec>=1'000'000'000) {
	    ts.tv_sec++;
	    ts.tv_nsec-=1'000'000'000;
	}
	pkt.next_sec=ts.tv_sec;
	pkt.next_nsec=ts.tv_nsec;

	pkt.dst_mac={
	    MY_DEST_MAC0, MY_DEST_MAC1, MY_DEST_MAC2,
	    MY_DEST_MAC3, MY_DEST_MAC4, MY_DEST_MAC5
	};
	pkt.dst_ip={192,168,0,6};
	pkt.dst_port=htons(3);

	pkt.src_mac={
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0],
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1],
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2],
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3],
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4],
	    ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5]
	};
	pkt.src_ip={192,168,0,2};
	pkt.type=htons(ETH_P_IP);

	pkt.src_port=htons(49895);
	pkt.id=0xb3a3;

	pkt.version_headerlength=0x45;	// IPv4
	pkt.services=0;
	pkt.checksum=0;
	pkt.flags_fragment_offset=0x40;	// Don't fragment
	pkt.ttl=64;
	pkt.protocol=ipv4_header_t::UDP;

	pkt.length=hton(sizeof(pkt)-sizeof(ethernet_t));
	pkt.udp_length=hton(sizeof(pkt)-sizeof(ipv4_t));
	pkt.do_ipv4_checksum();

	struct sockaddr_ll socket_address;
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	for(int i=0;i<pkt.dst_mac.size(); i++)
	    socket_address.sll_addr[1]=pkt.dst_mac[i];
	if (sendto(sockfd, &pkt, sizeof(pkt), 0,
	    (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)
	) < 0) {
	    printf("Send failed\n");
	    return NULL;
	}

	outb(0,base);
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);

	struct  __attribute__ ((__packed__)) rx_sync_t:public udp_t {
	    ::sync_t::sync_response data;
	} buffer;
        x.rx_size=recvfrom(rxs, &buffer, sizeof(buffer), MSG_DONTWAIT,
            NULL, NULL);
	if(clock_gettime(CLOCK_MONOTONIC, &timestamp)) {
	    perror("clock_gettime");
	    return NULL;
	}
	int dt=timestamp.tv_nsec-x.timestamp.tv_nsec;
	dt+=1'000'000'000*(timestamp.tv_sec-x.timestamp.tv_sec);
	x.dt=dt;
	if(x.rx_size==66) {
	    x.data=buffer.data;
	}

    }
    return NULL;
}

int main(int argc, char *argv[])
{

    sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if(sockfd==-1) {
	perror("socket");
	return 1;
    }

    rxs = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE));
    if(sockfd==-1) {
	perror("receive socket");
	return 1;
    }


    /* Get the index of the interface to send on */
    char ifName[IFNAMSIZ];
    if(argc>1)
	    strcpy(ifName, argv[1]);
    else
	    strcpy(ifName, DEFAULT_IF);
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
	perror("SIOCGIFINDEX");
	return 1;
    }

    int sockopt;
    if(setsockopt(rxs, SOL_SOCKET, SO_REUSEADDR, &sockopt,sizeof sockopt)<0){
	perror("setsockopt");
	return 1;
    }
    /* Bind to device */
    if(setsockopt(rxs, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1)<0){
	perror("SO_BINDTODEVICE");
	return 1;
    }

    /* Get the MAC address of the interface to send on */
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	perror("SIOCGIFHWADDR");

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
