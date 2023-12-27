#include "drive_io.h"
#include <unistd.h>

// spindle
constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x03, 0x08, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x85, 0x0c, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};

raw_socket skt("eth1");

int main(int argc, char *argv[])
{
    config_ns::to_drive d{
	.led=atoi(argv[1]),
	.encoder=atoi(argv[2]),
	.poles=atoi(argv[3]),	// ignored by PQ/MFS encoders
	.angle_offset=0.0,
	.limit_r=0.44, .limit_i=0.8
    };
    skt.send(config_ns::send_t(skt,dst_mac,dst_ip, d));
}
