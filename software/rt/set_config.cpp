#include "drive_io.h"
#include <unistd.h>
#include <iostream>

#if !(__cpp_lib_format >= 202106L)
#include <fmt/core.h>
#include <fmt/ranges.h>

namespace std { using fmt::format; }
#else
#include <format>
#endif

// spindle
std::array<uint8_t,6> mac{0xc2, 0x00, 0x03, 0x08, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> mac{0xc2, 0x00, 0x85, 0x0c, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};

/* Spindle motor
    0		don't care
    3		AMT21 encoder
    4		4 pole-pairs
    0.88	angle_offset

    limit=0.44+0.75i (stroom meting faalt >94% duty cycle)
    P=40, I=0.2,, L=1.55

    ./set_config c2:00:03:08:10:c0 0 3 4 0.88 0.44 0.75 40 0.2 1.55
*/
/* X motor
    0
    1
    0
    -0.04	angle_offset
    limit=0.42+0.77i
    P=35, I=1.5 L=1

    ./set_config c2:00:85:0c:10:c0 0 0 0 -0.04 0.42 0.77 35 1.5 1
*/
/* Z motor
    0
    1
    0
    -0.04	angle_offset
    limit=0.42+0.77i
    P=35, I=1.5 L=1

    ./set_config c2:00:8d:11:11:c0 0 0 0 0.14 0.44 0.75 35 1.5 1
*/

raw_socket skt("eth1");

auto parse_mac(char const *in)
{
    std::array<int,6> m;
    std::array<uint8_t,6> result;
    if(sscanf(in,"%x:%x:%x:%x:%x:%x",
	&m[0],&m[1],&m[2],&m[3],&m[4],&m[5])!=6
    ) {
	std::cerr << "Invalid mac address: " << in << "\n";
	for(auto &x: result)
	    x=0;
    } else {
	for(int i=0; i<result.size(); i++)
	    result[i]=m[i];
    }
    return result;
}

int main(int argc, char *argv[])
{
    if(argc<11) {
	std::cerr << "parameters:\n"
	    << "	mac	12:34:45:66:77:00\n"
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

    mac=parse_mac(argv[1]);
    if(!mac[0])
	return 1;

    std::cerr << std::format("{}\n",mac);

    auto fl=[&](char const *p) {
	return float(atof(p));
    };
    config_ns::to_drive d{
	.led=atoi(argv[2]),
	.encoder=atoi(argv[3]),
	.poles=atoi(argv[4]),	// ignored by PQ/MFS encoders
	.angle_offset=fl(argv[5]),
	.limit_r=fl(argv[6]), .limit_i=fl(argv[7]),
	.current_P=fl(argv[8]), .current_I=fl(argv[9]), .current_L=fl(argv[10])
    };
    skt.send(config_ns::send_t(skt,mac,dst_ip, d));
}
