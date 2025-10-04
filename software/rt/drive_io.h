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
	std::complex<float> I,
	std::complex<float> lim,
	uint32_t dout
    ):send_base_t(skt,mac,ip,port)
    {
	new_data++; new_data|=0x80000000;
	Iset[0]=I.real();
	Iset[1]=I.imag();
	limit[0]=lim.real();
	limit[1]=lim.imag();
	digout=dout;

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
	to_drive const &data
    ):send_base_t(skt,mac,ip,port), to_drive(data)
    {
	new_data++; new_data|=0x80000000;
	finish_packet(sizeof(*this));
    }
};

struct recv_t:public udp_t, public to_host {
};

}


////////////////////////////////////////////////////////////////////////////////
// Used for logging
////////////////////////////////////////////////////////////////////////////////

struct sync_t:public sync_ns::to_host, public motion_ns::to_host {
    timespec timestamp;
/*
    std::complex<float> I;
    decltype(controller)::input_t setpoint;
    decltype(controller)::input_t error;
*/
};

std::ostream &operator<<(std::ostream &s, sync_t const &d) {
    s << d.timestamp.tv_sec << " " << d.timestamp.tv_nsec	// 1,2
	<< " " << d.tx_seconds << " " << d.tx_nanoseconds	// 3,4
	<< " " << d.rx_seconds << " " << d.rx_nanoseconds	// 5,6
	<< " " << d.timer << " " << d.integrator		// 7,8
	<< " " << d.position	// 9
	<< " " << d.angle	// 10
	<< " " << d.encoder_invalid	// 11
	<< " " << d.encoder_missing	// 12
	<< " " << d.Irotor[0]	// 13
	<< " " << d.Irotor[1]	// 14
	<< " " << d.Vrotor[0]	// 15
	<< " " << d.Vrotor[1]	// 16
	<< " " << d.ADC[0]	// 17
	<< " " << d.ADC[1]	// 18
	<< " " << d.timer_delta	// 19
	<< " " << d.rx_counter	// 20
	<< " " << d.rx_data[0]	// 21
	<< " " << d.rx_data[1]	// 22
	<< " " << d.rx_data[2]	// 23
	<< " " << d.rx_data[3]	// 23
	<< " " << d.position2	// 25
	<< " " << d.index2	// 25
	<< " " << d.Vservo	// 27
	<< " " << d.rx_data[8]	// 28
	<< " " << d.rx_data[9]	// 29
	<< " " << d.rx_data[10]	// 30
	<< " " << d.rx_data[11]	// 31
	<< " " << d.rx_data[12]	// 32
	<< " " << d.timer_error	// 33
/*
	<< " " << d.setpoint(0)	// 28
	<< " " << real(d.I)	// 29
	<< " " << imag(d.I)	// 30
	<< " " << d.setpoint(1)	// 31
	<< " " << d.error(0)	// 32
	<< " " << d.error(1)	// 33
*/
	;
    return s;
}

#endif
