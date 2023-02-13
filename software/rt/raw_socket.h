#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>

#include <array>

class raw_socket {
    static constexpr uint16_t ETHER_TYPE=0x0800;
    int rxs;
public:
    struct sockaddr_ll socket_address;
    std::array<uint8_t,6> src_mac;
    std::array<uint8_t,4> src_ip{192,168,0,2};

public:
    raw_socket(char const *name) {
	rxs = ::socket(AF_PACKET, SOCK_RAW, htons(ETHER_TYPE));
	if(rxs==-1) {
	    perror("receive socket");
	    exit(1);
	}

	/* Get the index of the interface to send on */
	char ifName[IFNAMSIZ];
	memset(ifName,0,sizeof(ifName));
	strncpy(ifName,name,sizeof(ifName));

	int sockopt;
	if(setsockopt(rxs, SOL_SOCKET, SO_REUSEADDR,&sockopt,sizeof sockopt)<0){
	    perror("setsockopt");
	    exit(1);
	}
	/* Bind to device */
	if(setsockopt(rxs, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1)<0){
	    perror("SO_BINDTODEVICE");
	    exit(1);
	}

	struct ifreq if_idx;
	memset(if_idx.ifr_name,0,sizeof(if_idx.ifr_name));
	strncpy(if_idx.ifr_name, name, sizeof(if_idx.ifr_name));
	if (ioctl(rxs, SIOCGIFINDEX, &if_idx) < 0) {
	    perror("SIOCGIFINDEX");
	    exit(1);
	}
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;

	/* Get the MAC address of the interface to send on */
	if (ioctl(rxs, SIOCGIFHWADDR, &if_idx) < 0)
	    perror("SIOCGIFHWADDR");
	src_mac={
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[0],
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[1],
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[2],
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[3],
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[4],
	    ((uint8_t *)&if_idx.ifr_hwaddr.sa_data)[5]
	};
    }

    template <class T>
    int send(T const &pkt) {
	if(sendto(rxs, &pkt, sizeof(pkt), 0,
	    (struct sockaddr*)&socket_address, sizeof(socket_address) ) < 0
	)  {
	    fprintf(stderr,"Send failed\n");
	    return 1;
	}
	return 0;
    }

    int socket() { return rxs; }
};

#endif
