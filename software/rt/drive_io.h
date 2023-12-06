#ifndef DRIVE_IO_H
#define DRIVE_IO_H

#include <complex>
#include "raw_socket.h"
#include "drive_packet.h"

struct __attribute__ ((__packed__)) send_base_t:public udp_t {
    template <class T>
    send_base_t(
	T const &skt, std::array<uint8_t,6> mac, std::array<uint8_t,4> ip,
	uint16_t port
    ) {
	dst_mac=mac;
	dst_ip=ip;
	dst_port=htons(port);

	src_mac=skt.src_mac;
	src_ip=skt.src_ip;
	type=htons(ETH_P_IP);

	src_port=htons(port+49000);
	id=0xb3a3;

	version_headerlength=0x45;	// IPv4
	services=0;
	checksum=0;
	flags_fragment_offset=0x40;	// Don't fragment
	ttl=64;
	protocol=ipv4_header_t::UDP;
    }

    void finish_packet(size_t size) {
	length=hton(size-sizeof(ethernet_t));
	udp_length=hton(size-sizeof(ipv4_t));
	do_ipv4_checksum();
    }
};

////////////////////////////////////////////////////////////////////////////////
// Sync  packets
////////////////////////////////////////////////////////////////////////////////

namespace sync_ns {

struct __attribute__ ((__packed__)) send_t:public send_base_t, public to_drive
{
    template <class T>
    send_t(
	T const &skt, std::array<uint8_t,6> mac, std::array<uint8_t,4> ip,
	struct timespec now, timespec next
    ):send_base_t(skt,mac,ip,port)
    {
	tx_seconds=now.tv_sec;
	tx_nanoseconds=now.tv_nsec;
	rx_seconds=next.tv_sec;
	rx_nanoseconds=next.tv_nsec;

	finish_packet(sizeof(*this));
    }
};

struct recv_t:public udp_t, public to_host {
};

}

////////////////////////////////////////////////////////////////////////////////
// motion packets
////////////////////////////////////////////////////////////////////////////////
namespace motion_ns {

struct __attribute__ ((__packed__)) send_t:public send_base_t, public to_drive
{
    template <class T>
    send_t(
	T const &skt, std::array<uint8_t,6> mac, std::array<uint8_t,4> ip,
	std::complex<float> I
    ):send_base_t(skt,mac,ip,port)
    {
	new_data++; new_data|=0x80000000;
	Iset[0]=I.real();
	Iset[1]=I.imag();

	finish_packet(sizeof(*this));
    }
};

struct recv_t:public udp_t, public to_host {
};

}

////////////////////////////////////////////////////////////////////////////////
// config packets
////////////////////////////////////////////////////////////////////////////////
namespace config_ns {

struct __attribute__ ((__packed__)) send_t:public send_base_t, public to_drive
{
    template <class T>
    send_t(
	T const &skt, std::array<uint8_t,6> mac, std::array<uint8_t,4> ip,
	int led_in
    ):send_base_t(skt,mac,ip,port)
    {
	new_data++; new_data|=0x80000000;
	led=led_in;
	finish_packet(sizeof(*this));
    }
};

struct recv_t:public udp_t, public to_host {
};

}


#endif
