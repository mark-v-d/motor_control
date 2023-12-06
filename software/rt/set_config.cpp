#include "drive_io.h"
#include <unistd.h>

constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x85, 0x0c, 0x10, 0xc0};
//constexpr std::array<uint8_t,6> dst_mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0};
constexpr std::array<uint8_t,4> dst_ip{192,168,0,6};

raw_socket skt("eth1");

int main(int argc, char *argv[])
{
    config_ns::send_t pkt(skt,dst_mac,dst_ip, atoi(argv[1]));
    for(;;)  {
	skt.send(pkt);
	usleep(100);
    }
}
