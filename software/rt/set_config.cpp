#include "drive_io.h"
#include <unistd.h>
#include <iostream>

// spindle
constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x03, 0x08, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x85, 0x0c, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};

/* Spindle motor
    0		don't care
    3		AMT21 encoder
    4		4 pole-pairs
    0.88	angle_offset

    P=40, I=0.2,, L=1.55 (Nieuwe versie met voltage scaling
    limit=0.44+0.79i (stroom meting faalt >94% duty cycle)
*/

raw_socket skt("eth1");

int main(int argc, char *argv[])
{
    if(argc<10) {
	std::cerr << "parameters:\n"
	    << "	LED	integer 0:3\n"
	    << "	encoder	integer\n"
	    << "		1=mitsubishi_PQ_t\n"
	    << "		2=mitsubishi_MFS13_t\n"
	    << "		3=AMT21_t\n"
	    << "	poles	integer>0\n"
	    << "	angle_offset	float\n"
	    << "	limit_r	float real part of duty cycle limiter\n"
	    << "	limit_i	float imaginary part of duty cycle limiter\n"
	    << "	P	float Current control proportional gain\n"
	    << "	I	float Current control integrating gain\n"
	    << "	L	float Current control limit recovery gain\n"
	    ;
	return 1;
    }

    auto fl=[&](char const *p) {
	return float(atof(p));
    };
    config_ns::to_drive d{
	.led=atoi(argv[1]),
	.encoder=atoi(argv[2]),
	.poles=atoi(argv[3]),	// ignored by PQ/MFS encoders
	.angle_offset=fl(argv[4]),
	.limit_r=fl(argv[5]), .limit_i=fl(argv[6]),
	.current_P=fl(argv[7]), .current_I=fl(argv[8]), .current_L=fl(argv[9])
    };
    skt.send(config_ns::send_t(skt,dst_mac,dst_ip, d));
}
